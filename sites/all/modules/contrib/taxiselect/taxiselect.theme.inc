<?php
//$Id: taxiselect.theme.inc,v 1.1.2.4 2009/11/19 11:02:09 aland Exp $

/**
 * @file
 * Provides a number of theming functions related to the form and error messages.
 *
 */

/**
 * Themes a list of term suggestions for any given taxomony.
 *
 * @param $vocabulary
 *   The vocabulary.
 * @param $value
 *   The search term string.
 * @param $type
 *   A flag to see what function called this.
 * @return
 *   A string containing the suggestions.
 */
function theme_taxiselect_message_suggestions($vocabulary = NULL, $value = '', $type = 'not_found') {
  $suggestions = '';
  $limit = variable_get('taxiselect_suggestions_limit', 3);
  if ($vocabulary->vid && !empty($value) && $limit >= 1) {
    $matches = array();
    foreach (taxiselect_suggestions($vocabulary->vid, $value, $limit) as $item) {
      $items = taxiselect_escape_terms($item);
      if (variable_get('taxiselect_reverse_lookup', 0)) {
        $items = array_reverse($items);
      }

      $matches[] = implode(', ', $items);
    }
    if (count($matches) == 1) {
      $suggestions = t('Did you mean %suggestion?', array('%suggestion' => $matches[0]));
    }
    elseif (count($matches)) {
      $suggestions = t('Did you mean one of the following? !suggestions', array('!suggestions' => theme('item_list', $matches)));
    }
    else {
      $suggestions = t('No suggestions were found.');
    }
  }
  return $suggestions;
}

/**
 * Themes the multiple match error messages.
 *
 * This checks for a list of possible matches and then
 * uses an optional user supplied message to construct
 * the error message.
 *
 * @param $vocabulary
 *   The vocabulary.
 * @param $value
 *   The search term string.
 * @return
 *   A string containing the multiple match error message.
 */
function theme_taxiselect_message_multiple_matches($vocabulary = NULL, $value = '') {
  if ($vocabulary->vid && $message = trim(variable_get('taxiselect_message_multiple_matches_'. $vocabulary->vid, ''))) {
    $message = check_plain($message);
    $suggestions = theme('taxiselect_message_suggestions', $vocabulary->vid, $value, 'multiple_matches');
    $message = str_replace('[suggestions]', $suggestions, $message);
    $message = str_replace('[vocabulary]', check_plain($vocabulary->name), $message);
    $message = str_replace('[term]', check_plain($value), $message);
  }
  else {
    $message = t('Multiple possible matches were found for the %term in %vocabulary.', array('%term' => $value, '%vocabulary' => $vocabulary->name));
  }
  return trim($message);
}

/**
 * Themes the term not found error messages.
 *
 * This checks for a list of possible matches and then
 * uses an optional user supplied message to construct
 * the error message.
 *
 * @param $vocabulary
 *   The vocabulary.
 * @param $value
 *   The search term string.
 * @return
 *   A string containing the term not found error message.
 */
function theme_taxiselect_message_not_found($vocabulary = NULL, $value = '') {
  if ($vocabulary->vid && $message = trim(variable_get('taxiselect_message_not_found_'. $vocabulary->vid, ''))) {
    $message = check_plain($message);
    $suggestions = theme('taxiselect_message_suggestions', $vocabulary->vid, $value, 'not_found');
    $message = str_replace('[suggestions]', $suggestions, $message);
    $message = str_replace('[vocabulary]', check_plain($vocabulary->name), $message);
    $message = str_replace('[term]', check_plain($value), $message);
  }
  else {
    $message = t('The term %term could not be found in %vocabulary.', array('%term' => $value, '%vocabulary' => $vocabulary->name));
  }
  return trim($message);
}
