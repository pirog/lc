<?php
// $Id: taxiselect.admin.inc,v 1.1.2.1.2.6 2009/11/19 11:02:09 aland Exp $

/**
 * @file
 * Administration related functions.
 */

/**
 * General settings form.
 */
function taxiselect_admin_settings() {
  $form['taxonomy_terms_per_page_admin'] = array(
    '#type' => 'select',
    '#title' => t('Administration terms per page'),
    '#default_value' => variable_get('taxonomy_terms_per_page_admin', 100),
    '#options' => drupal_map_assoc(array(10, 15, 20, 25, 50, 75, 100, 125, 150, 200, 250)),
    '#description' => t('This is a global flag to change the number of terms per page listed on the "list terms" admin page.'),
    '#required' => TRUE,
  );

  $form['taxiselect_suggestions_limit'] = array(
    '#type' => 'select',
    '#title' => t('Taxiselect suggestion limit'),
    '#default_value' => variable_get('taxiselect_suggestions_limit', 3),
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25)),
    '#description' => t('Shows a number of suggestions if the term matches multiple values or is not found on form submission. You must use the string [suggestions] in the forms error message strings for these to show.'),
    '#required' => TRUE,
  );
  $form['taxiselect_trigger_length'] = array(
    '#type' => 'select',
    '#title' => t('Taxiselect trigger length'),
    '#default_value' => variable_get('taxiselect_trigger_length', 3),
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
    '#description' => t('Minimum number of characters before returning an AJAX response. Useful to reduce server load.'),
    '#required' => TRUE,
  );
  $form['taxiselect_response_limit'] = array(
    '#type' => 'select',
    '#title' => t('Taxiselect response limit'),
    '#default_value' => variable_get('taxiselect_response_limit', 10),
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 40, 50, 75, 100, 150, 200, 250, 300, 400, 500)),
    '#description' => t('Maximum number of results to return per AJAX response. Useful to increase response times/reduce server load or to offer more choices to the user.'),
    '#required' => TRUE,
  );
  $form['taxiselect_reverse_lookup'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable reverse lookups'),
    '#default_value' => variable_get('taxiselect_reverse_lookup', 0),
    '#description' => t('This reverses the autocomplete lookup from "Thai (new), Food" to "Food, Thai (new)".'),
  );
  $form['taxiselect_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types to associate this widget with'),
    '#default_value' => array_filter(variable_get('taxiselect_node_types', array())),
    '#options' => array_map('check_plain', node_get_types('names')),
    '#description' => t('A list of node content types you want to use the taxiselect element with.'),
  );

  $vocabulary_options = array();
  $vocabularies = taxonomy_get_vocabularies();
  $form['#vocabularies'] = $vocabularies;
  foreach ($vocabularies as $vocabulary) {
    $vocabulary_options[$vocabulary->vid] = check_plain($vocabulary->name);
  }
  $form['taxiselect_vocabularies'] = array(
    '#type' => 'checkboxes',
    '#default_value' => array_filter(variable_get('taxiselect_vocabularies', array())),
    '#options' => $vocabulary_options,
  );

  $form['taxiselect_admin_vocabularies'] = array(
    '#type' => 'checkboxes',
    '#default_value' => array_filter(variable_get('taxiselect_admin_vocabularies', array())),
    '#options' => $vocabulary_options,
  );
  $form['taxiselect_free_tagging'] = array(
    '#type' => 'checkboxes',
    '#options' => $vocabulary_options,
    '#default_value' => variable_get('taxiselect_free_tagging', array()),
  );
  $form['taxiselect_skip_root'] = array(
    '#type' => 'checkboxes',
    '#default_value' => variable_get('taxiselect_skip_root', array()),
    '#options' => $vocabulary_options,
  );
  $form['taxiselect_autocomplete_only'] = array(
    '#type' => 'checkboxes',
    '#default_value' => variable_get('taxiselect_autocomplete_only', array()),
    '#options' => $vocabulary_options,
  );
  // custom error messages
  foreach ($vocabularies as $vocabulary) {
    $form['taxiselect_message_not_found_'. $vocabulary->vid] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('taxiselect_message_not_found_'. $vocabulary->vid, ''),
    );
    $form['taxiselect_message_multiple_matches_'. $vocabulary->vid] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('taxiselect_message_multiple_matches_'. $vocabulary->vid, ''),
    );
  }

  $form = system_settings_form($form);
  // TODO - look at moving to advanced help
  $tmp_vocab = new StdClass();
  $tmp_vocab->name = t('EXAMPLE VOCAB NAME');
  $tmp_vocab->vid = NULL;
  $tmp_value = t('EXAMPLE USER INPUT');
  $form['taxiselect_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['taxiselect_help']['free_tagging'] = array(
    '#type' => 'item',
    '#title' => t('Pseudo free tagging'),
    '#value' => '<p>'. t('This option provides a way to free tag new terms into hierarchical vocabularies.') .'</p><p>'. t('This option can be used to create one new term at a time when submitting the form or by using the %add or %replace buttons. This term will be saved even if the page is not saved. If the "%hide" option is selected, the new term is appended to the first parental match found.', array('%hide' => t('Hide top level categories.'), '%add' => t('Add'), '%replace' => t('Replace'))) .'</p>',
  );
  $form['taxiselect_help']['skip_root'] = array(
    '#type' => 'item',
    '#title' => t('Hiding top level categories'),
    '#value' => '<p>'. t('If enabled, the root level terms will not be visible.') .'</p><p>'. t('For example, if the root level terms are continents, such as Asia, and the second level terms are countries, this option will hide all continental terms. This removes the ability to select root level terms. This option is ignored on administration pages.') .'</p>',
  );
  $form['taxiselect_help']['not_found'] = array(
    '#type' => 'item',
    '#title' => t('Custom term not found message'),
    '#value' => '<p>'. t('This allows you to override the standard error message of:') .'</p>'
      .'<ul><li>'. theme('taxiselect_message_not_found', $tmp_vocab, $tmp_value) .'</li><ul>'
      .'<p>' .t('Three tokens are available:') .'</p>'
      . theme('item_list', array(
          t('!token The suggestions found when there are no matches or multiple matches.<br/>See %func to theme the suggestion results.', array('!token' => '<strong>[suggestions]</strong>', '%func' => 'theme_taxiselect_message_suggestions()')),
          t('!token The vocabulary name.', array('!token' => '<strong>[vocabulary]</strong>')),
          t('!token The user submitted value.', array('!token' => '<strong>[term]</strong>')),
        )),
  );
  $form['taxiselect_help']['multiple_matches'] = array(
    '#type' => 'item',
    '#title' => t('Custom multiple matches message'),
    '#value' => '<p>'. t('This allows you to override the standard error message of:') .'</p>'
      .'<ul><li>'. theme('taxiselect_message_multiple_matches', $tmp_vocab, $tmp_value) .'</li><ul>'
      .'<p>'. t('The three tokens !suggestion, !vocab, and !term are available.', array('!suggestion' => '<strong>[suggestions]</strong>', '!vocab' => '<strong>[vocabulary]</strong>', '!term' => '<strong>[term]</strong>')) .'</p>',
  );

  $form['#theme'] = 'taxiselect_admin_settings';
  return $form;
}

// The taxonomy term form.
function _taxiselect_form_taxonomy_form_term_alter(&$form, &$form_state) {
  // Don't alter the form when it's in confirmation mode.
  if (isset($form_state['confirm_delete']) || isset($form_state['confirm_parents'])) {
    return;
  }
  $vocabulary = (object) $form['#vocabulary'];
  $parent = array_keys(taxonomy_get_parents($form['tid']['#value']));
  $relations = array_keys(taxonomy_get_related($form['tid']['#value']));
  // A term can't be the child of itself, nor of its children.
  $children = taxonomy_get_tree($vocabulary->vid, $form['tid']['#value']);
  foreach ($children as $child) {
    $exclude[] = $child->tid;
  }
  $exclude[] = $form['tid']['#value'];

  $taxiselect_vocabularies = array_filter(variable_get('taxiselect_admin_vocabularies', array()));
  if (in_array($vocabulary->vid, $taxiselect_vocabularies)) {
    $form['advanced']['parent'] = array(
      '#title' => t('Parents'),
      '#type' => 'taxiselect_element',
      '#vocabulary' => $vocabulary,
      '#multiple' => 0,
      '#default_value' => $parent,
      '#exclude' => $exclude,
    );
    $form['advanced']['relations'] = array(
      '#title' => t('Related terms'),
      '#type' => 'taxiselect_element',
      '#vocabulary' => $vocabulary,
      '#multiple' => 0,
      '#default_value' => $relations,
      '#exclude' => array($form['tid']['#value']),
    );
  }
  else {
    $form['advanced']['parent'] = _taxonomy_term_select(t('Parents'), 'parent', $parent, $vocabulary->vid, t('Parent terms') .'.', 1, '<'. t('root') .'>', $exclude);
    $form['advanced']['relations'] = _taxonomy_term_select(t('Related terms'), 'relations', $relations, $vocabulary->vid, NULL, 1, '<'. t('none') .'>', array($form['tid']['#value']));
  }
}
