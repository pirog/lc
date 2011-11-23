<?php
//$Id: taxiselect.pages.inc,v 1.1.2.6 2009/11/19 11:02:09 aland Exp $

/**
 * @file
 */

function _taxiselect_node_form_alter(&$form, &$form_state) {
  $node = $form['#node'];
  // The node types and vocabularies that we are interested in.
  $taxiselect_vocabularies = array();
  if (in_array($form['type']['#value'], array_filter(variable_get('taxiselect_node_types', array())))) {
    $taxiselect_vocabularies = array_filter(variable_get('taxiselect_vocabularies', array()));
  }

  if (!isset($node->taxonomy)) {
    $terms = empty($node->nid) ? array() : taxonomy_node_get_terms($node);
  }
  else {
    // After preview the terms must be converted to objects.
    if (isset($form_state['node_preview'])) {
      $node->taxonomy = taxonomy_preview_terms($node);
    }
    $terms = $node->taxonomy;
  }

  $c = db_query(db_rewrite_sql("SELECT v.* FROM {vocabulary} v INNER JOIN {vocabulary_node_types} n ON v.vid = n.vid WHERE n.type = '%s' ORDER BY v.weight, v.name", 'v', 'vid'), $node->type);

  while ($vocabulary = db_fetch_object($c)) {
    // CnP from taxomony module
    if ($vocabulary->tags) {
      if (isset($form_state['node_preview'])) {
        // Typed string can be changed by the user before preview,
        // so we just insert the tags directly as provided in the form.
        $typed_string = $node->taxonomy['tags'][$vocabulary->vid];
      }
      else {
        $typed_string = taxonomy_implode_tags($terms, $vocabulary->vid) . (array_key_exists('tags', $terms) ? $terms['tags'][$vocabulary->vid] : NULL);
      }
      if ($vocabulary->help) {
        $help = filter_xss_admin($vocabulary->help);
      }
      else {
        $help = t('A comma-separated list of terms describing this content. Example: funny, bungee jumping, "Company, Inc.".');
      }
      $form['taxonomy']['tags'][$vocabulary->vid] = array('#type' => 'textfield',
        '#title' => $vocabulary->name,
        '#description' => $help,
        '#required' => $vocabulary->required,
        '#default_value' => $typed_string,
        '#autocomplete_path' => 'taxonomy/autocomplete/'. $vocabulary->vid,
        '#weight' => $vocabulary->weight,
        '#maxlength' => 1024,
      );
    }
    else {
      // Extract terms belonging to the vocabulary in question.
      $default_terms = array();
      foreach ($terms as $term) {
        // Free tagging has no default terms and also no vid after preview.
        if (isset($term->vid) && $term->vid == $vocabulary->vid) {
          $default_terms[$term->tid] = $term;
        }
      }
      if (in_array($vocabulary->vid, $taxiselect_vocabularies)) {
        // The AHAH form_state overrides the $node->taxonomy.
        if (isset($form_state['values']['taxonomy'][$vocabulary->vid]) && is_array($form_state['values']['taxonomy'][$vocabulary->vid])) {
          $default_terms = array();
          foreach ($form_state['values']['taxonomy'][$vocabulary->vid] as $key => $value) {
            if (is_numeric($key)) {
              $default_terms[$key] = $key;
            }
          }
        }
        $form['taxonomy'][$vocabulary->vid] = array(
          '#type' => 'taxiselect_element',
          '#vocabulary' => $vocabulary,
          '#default_value' => array_keys($default_terms),
        );
      }
      else {
        $form['taxonomy'][$vocabulary->vid] = taxonomy_form($vocabulary->vid, array_keys($default_terms), filter_xss_admin($vocabulary->help));
      }
      $form['taxonomy'][$vocabulary->vid]['#required'] = $vocabulary->required;
      $form['taxonomy'][$vocabulary->vid]['#weight'] = $vocabulary->weight;
    }
  }
  if (!empty($form['taxonomy']) && is_array($form['taxonomy'])) {
    if (count($form['taxonomy']) > 1) {
      // Add fieldset only if form has more than 1 element.
      $form['taxonomy'] += array(
        '#type' => 'fieldset',
        '#title' => t('Vocabularies'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
    }
    $form['taxonomy']['#weight'] = -3;
    $form['taxonomy']['#tree'] = TRUE;
  }
}

/**
 * Menu callback for AHAH additions.
 */
function taxiselect_add_ahah($parents) {
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'taxonomy', 'taxonomy.admin');

  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $_POST;
  $form = form_builder($form_id, $form, $form_state);

  $_parents = $parents;
  $parents = explode('][', $parents);
  $target_form = $form;
  while ($key = array_shift($parents)) {
    $target_form = $target_form[$key];
  }
  // Clear the element if no errors found
  if (!form_get_errors()) {
    $target_form['add']['taxiselect']['#value'] = '';
  }
  unset($target_form['#prefix'], $target_form['#suffix']);
  // ahah returns the full element and messages
  $output = theme('status_messages') . drupal_render($target_form);

  drupal_json(array('status' => TRUE, 'data' => $output));
}

/**
 * Core functionality for the autocomplete response.
 *
 * @param $vid The vocab vid.
 * @param $string The string to search for.
 */
function taxiselect_autocomplete($vid, $string = '') {
  $results = array();
  if (trim($string)) {
    foreach (taxiselect_suggestions($vid, $string, variable_get('taxiselect_response_limit', 10)) as $match) {
      $match = taxiselect_escape_terms($match);
      if (variable_get('taxiselect_reverse_lookup', 0)) {
        $match = array_reverse($match);
      }

      $results[implode(', ', $match)] = theme('taxiselect_suggestion', $match, $vid);
    }

    if (empty($results) && taxiselect_free_tagging($vid)) {
      foreach (taxiselect_suggestions($vid, $string, variable_get('taxiselect_response_limit', 10), TRUE) as $match) {
        $new_term = array_shift($match);
        $keys = $match = taxiselect_escape_terms($match);
        array_unshift($match, taxiselect_escape_terms(t('!term (new)', array('!term' => $new_term))));
        array_unshift($keys, taxiselect_escape_terms($new_term));
        if (variable_get('taxiselect_reverse_lookup', 0)) {
          $match = array_reverse($match);
          $keys = array_reverse($keys);
        }
        $results[implode(', ', $keys)] = theme('taxiselect_suggestion', $match, $vid);
      }
    }
  }
  drupal_json($results);
}
