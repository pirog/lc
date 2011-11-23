<?php
//$Id: taxiselect.elements.inc,v 1.1.2.7 2009/11/19 11:02:09 aland Exp $

/**
 * @file
 * This contains the functinality of the taxiselect element.
 */

/**
 * Implementation of hook_elements().
 */
function taxiselect_elements() {
  $type['taxiselect_element'] = array(
    '#input' => TRUE,
    '#exclude' => array(),
    '#process' => array('taxiselect_element_expand', 'form_expand_ahah'),
    '#default_value' => array(),
    '#element_validate' => array('taxiselect_element_validate'),
    '#return_value' => 1,
    '#delta' => 0,
  );
  return $type;
}

/**
 * The remove action is handled via form_type_HOOK_value().
 */
function taxiselect_element_validate($element, &$form_state) {
  $target_element = &$form_state['values'];
  $parents = $element['#parents'];
  while ($parent = array_shift($parents)) $target_element = &$target_element[$parent];
  unset($target_element['add']);
  unset($target_element['terms']);
}

/**
 * Implementation of form_type_hook_value().
 *
 * Adds the ability to accept term objects, arrays or tid's and
 * loads the form js / css files.
 */
function form_type_taxiselect_element_value($element, $edit = FALSE) {
  static $load = TRUE;
  if ($load) {
    drupal_add_css(drupal_get_path('module', 'taxiselect') .'/taxiselect.css');
    $load = FALSE;
  }
  $tids = array();
  if (empty($element['#vocabulary']) || !is_object($element['#vocabulary'])) {
    return $tids;
  }

  if ($edit !== FALSE) {
    if (is_array($edit['terms'])) {
      foreach ($edit['terms'] as $tid => $data) {
        if (!empty($tid) && (!isset($data['remove']) || $data['remove'] != 1)) {
          $tids[$tid] = $tid;
        }
      }
    }
    if (_taxiselect_is_ahah()) {
      if (implode('][', $element['#array_parents']) != arg(3)) {
        return $tids;
      }
    }

    $edit['add']['taxiselect'] = trim($edit['add']['taxiselect']);
    if (!empty($edit['add']['taxiselect'])) {
      $vocabulary = $element['#vocabulary'];
      $multiple = $element['#multiple'] ? $element['#multiple'] : ($vocabulary->multiple ? 0 : 1);

      // Check the limit first before trying to find / create the term to add.
      // The check using $replace is for the special case of the replace button.
      $replace = ($multiple == 1 && count($tids) >= 1);
      if ($replace || !$multiple || $multiple > count($tids)) {
        $search_terms = taxiselect_explode_tags($edit['add']['taxiselect'], FALSE);
        if (variable_get('taxiselect_reverse_lookup', 0)) {
          $search_terms = array_reverse($search_terms);
        }
        $new_tid = FALSE;
        if ($items = taxiselect_match_exact_path($search_terms, $vocabulary->vid, 1, FALSE, TRUE, $element['#exclude'])) {
          $new_tid = key($items[0]);
        }
        elseif ($_tid = taxiselect_inline_free_tagging($vocabulary, $search_terms, $element['#exclude'])) {
          $new_tid = $_tid;
        }
        if ($new_tid) {
          if ($replace) {
            $tids = array($new_tid => $new_tid);
          }
          else {
            $tids[$new_tid] = $new_tid;
          }
        }
        else {
          $error_path = isset($element['#parents']) ? array_merge($element['#parents'], array('add][taxiselect')) : array('add][taxiselect');
          if (count($element['#exclude']) && taxiselect_match_exact_path($search_terms, $vocabulary->vid, 1, FALSE, TRUE)) {
            if (implode('][', $element['#array_parents']) == 'advanced][relations') {
              form_set_error(implode('][', $error_path), t("A term can't be related to itself."));
            }
            else {
              form_set_error(implode('][', $error_path), t("A term can't be the child of itself, nor of its children."));
            }
          }
          else {
            form_set_error(implode('][', $error_path), theme('taxiselect_message_not_found', $vocabulary, $edit['add']['taxiselect']));
          }
        }
      }
      else {
        $error_path = isset($element['#parents']) ? array_merge($element['#parents'], array('add][taxiselect')) : array('add][taxiselect');
        form_set_error(implode('][', $error_path), t('The term %term could not be added as it would exceeded the maximum number of allowed values !count for %vocab.', array('%term' => $edit['add']['taxiselect'], '%vocab' => $vocabulary->name, '!count' => $multiple)));
      }
    }

  }
  else {
    $tids = drupal_map_assoc(array_filter(is_array($element['#default_value']) ? $element['#default_value'] : array($element['#default_value'])));
  }

  return $tids;
}

function taxiselect_inline_free_tagging($vocabulary, $search_terms, $exclude) {
  if (taxiselect_free_tagging($vocabulary->vid)) {
    $free_tagged_term = array_shift($search_terms);

    // We only support singular free tagging, if it has more items these must match
    // a existing path.
    if (count($search_terms)) {
      if ($parent_items = taxiselect_match_exact_path($search_terms, $vocabulary->vid, 1, FALSE, TRUE, $exclude)) {
        $ptid = key($parent_items[0]);
      }
      else {
        // Not found
        $ptid = FALSE;
      }
    }
    else {
      // Top level
      $ptid = 0;
      // Finds the first parental item to append the element.
      if (taxiselect_skip_root($vocabulary->vid)) {
        $ptid = (int) db_result(db_query("SELECT d.tid FROM {term_data} d INNER JOIN {term_hierarchy} h ON (d.tid = h.tid) WHERE d.vid = %d AND h.parent = 0", $vocabulary->vid));
      }
    }
    // Create the new term and assign this to the node.
    if ($ptid !== FALSE) {
      $term = array(
        'vid' => $vocabulary->vid,
        'parent' => array($ptid),
        'description' => '',
        'name' => $free_tagged_term,
      );

      if (taxonomy_save_term($term)) {
        drupal_set_message(t("The value %term was added to %vocab.", array('%term' => $term['name'], '%vocab' => $vocabulary->name)));
        return $term['tid'];
      }
    }
  }
  return FALSE;
}

function taxiselect_element_expand($element) {
  if (empty($element['#vocabulary']) || !is_object($element['#vocabulary'])) {
    return array(
      '#title' => isset($element['#title']) ? $element['#title'] : t('Invalid vocabulary'),
      '#type' => 'item',
      '#value' => t('Error: No defined vocabulary!'),
    );
  }
  $value = is_array($element['#value']) ? $element['#value'] : array();

  if (!isset($element['#multiple'])) {
    // 0 == Unlimited, 1+ == Max #num
    $element['#multiple'] = $element['#vocabulary']->multiple ? 0 : 1;
  }
  if (!isset($element['#autocomplete_only'])) {
    $element['#autocomplete_only'] = taxiselect_autocomplete_only($element['#vocabulary']->vid);
  }
  // This option uses the first avaliable term, all other terms will be
  // lost, so this option is limited to single selects vocabs.
  if ($element['#multiple'] != 1 && $element['#autocomplete_only']) {
    $element['#autocomplete_only'] = FALSE;
  }

  $element += array(
    '#title' => check_plain($element['#vocabulary']->name),
    '#prefix' => '<div class="taxiselect" id="taxiselect-'. $element['#vocabulary']->vid .'-'. $element['#id'] .'-ahah-wrapper">',
    '#suffix' => '</div>',
    '#required' => $element['#vocabulary']->required,
  );
  $element['#tree'] = TRUE;

  $element['add'] = array(
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>'
  );

  $element['add']['taxiselect'] = array(
    '#type' => 'textfield',
    '#description' => filter_xss_admin($element['#vocabulary']->help),
    '#autocomplete_path' => 'taxiselect/autocomplete/'. (_taxiselect_is_admin_path() ? 'admin/' : 'std/'). $element['#vocabulary']->vid,
    '#prefix' => '<div class="taxiselect-autocomplete">',
    '#suffix' => '</div>',
  );

  if ($element['#autocomplete_only']) {
    while ($tid = array_shift($value)) {
      if ($term = taxonomy_get_term($tid)) {
        if ($term_parents = taxiselect_parent_path($term)) {
          if (variable_get('taxiselect_reverse_lookup', 0)) {
            $term_parents = array_reverse($term_parents);
          }
          $element['add']['taxiselect']['#default_value'] = implode(', ', taxiselect_escape_terms($term_parents, FALSE));
          break;
        }
      }
    }
  }
  else {
    $element['add']['taxiselect_add'] = array(
      '#type' => 'submit',
      '#value' => ($element['#multiple'] == 1 && count($element['#value']) >= 1) ? t('Replace') : t('Add'),
      '#attributes' => array('class' => 'taxiselect-add'),
      '#ahah' => array(
        'path' => 'taxiselect/ahah/'. (_taxiselect_is_admin_path() ? 'admin/' : 'std/') . implode('][', $element['#array_parents']),
        'wrapper' => 'taxiselect-'. $element['#vocabulary']->vid .'-'. $element['#id'] . '-ahah-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
    $element['terms'] = _taxiselect_element_terms_form($element['#vocabulary'], $value, array_merge($element['#parents'], array('terms')));
  }
  return $element;
}

function _taxiselect_element_terms_form($vocabulary, $values, $parents) {
  $form = array(
    '#prefix' => '<div class="clear-block selected-terms" id="taxiselect-'. $vocabulary->vid .'-terms">',
    '#suffix' => '</div>',
  );
  $odd = TRUE;
  while ($tid = array_shift($values)) {
    if (!empty($tid) && $term = taxonomy_get_term($tid)) {
      $term_path = 'Term: Path not found';
      if ($term_parents = taxiselect_parent_path($term)) {
        if (variable_get('taxiselect_reverse_lookup', 0)) {
          $term_parents = array_reverse($term_parents);
        }
        $term_path = theme('taxiselect_suggestion', taxiselect_escape_terms($term_parents, TRUE));
      }
      $form[$term->tid] = array(
        '#prefix' => '<div class="taxiselect-selected-terms clear-block '. ($odd ? 'odd' : 'even') .'">',
        '#suffix' => '</div>',
      );
      $form[$term->tid]['remove'] = array(
        '#title' => t('Remove'),
        '#type' => 'checkbox',
        '#default_value' => 0,
      );
      // Ensures that the current value is correctly passed around
      $form[$term->tid]['tid'] = array(
        '#type' => 'hidden',
        '#default_value' => $term->tid,
      );
      $form[$term->tid]['name'] = array(
        '#value' => '<span class="taxiselect-term-path">'. $term_path .'</span>',
      );
      $odd = !$odd;
    }
  }
  if (count($form) == 2) {
    $form[$term->tid] = array(
      '#prefix' => '<div class="taxiselect-selected-terms clear-block odd">',
      '#value' => '<span class="taxiselect-no-terms">'. t('No terms are selected.') .'</span>',
      '#suffix' => '</div>',
    );
  }
  return $form;
}

/**
 * Theme function to format the output.
 */
function theme_taxiselect_element($element) {
  return theme('form_element', $element, '<div class="taxiselect">'. $element['#children'] .'</div>');
}
