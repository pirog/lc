<?php
//$Id: taxiselect-admin-settings.tpl.php,v 1.1.2.2 2009/11/19 11:02:09 aland Exp $

/**
 * @file
 */
print '<div class ="taxiselect-settings">';
print '<h3>'. t('Global settings') .'</h3>';
print drupal_render($form['taxonomy_terms_per_page_admin']);

print '<h3>'. t('Global TaxiSelect settings') .'</h3>';
print drupal_render($form['taxiselect_reverse_lookup']);
print drupal_render($form['taxiselect_suggestions_limit']);
print drupal_render($form['taxiselect_trigger_length']);
print drupal_render($form['taxiselect_response_limit']);
print drupal_render($form['taxiselect_node_types']);

$options = array(
  'taxiselect_vocabularies' => t('Use TaxiSelect for the selected content types'),
  'taxiselect_admin_vocabularies' => t('Use TaxiSelect for administrating this vocabulary'),
  'taxiselect_free_tagging' => t('Enable pseudo free tagging'),
  'taxiselect_skip_root' => t('Hide top level categories'),
  'taxiselect_autocomplete_only' => t('Use the reduced autocomplete when using single select vocabularies.'),
);
$keyed_options = array(
  'taxiselect_message_not_found_' => t('Custom term not found message'),
  'taxiselect_message_multiple_matches_' => t('Custom multiple matches message'),
);
print '<h3>'. t('Vocabulary level TaxiSelect settings') .'</h3>';
print drupal_render($form['taxiselect_help']);
foreach ($form['#vocabularies'] as $vid => $vocabulary) {
  print '<h4>'. t('!vocabulary settings', array('!vocabulary' => check_plain($vocabulary->name))) .'</h4>';
  foreach ($options as $key => $title) {
    $form[$key][$vid]['#title'] = $title;
    print drupal_render($form[$key][$vid]);
  }
  foreach ($keyed_options as $key => $title) {
    $form[$key . $vid]['#title'] = $title;
    print drupal_render($form[$key . $vid]);
  }
}
print drupal_render($form);
print '</div>';