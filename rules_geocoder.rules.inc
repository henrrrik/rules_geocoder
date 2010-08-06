<?php
// $Id

/**
 * @file
 * Rules integration for Rules Geocoder
 */

/**
 * Implementation of hook_rules_action_info().
 * @ingroup rules
 */
function rules_geocoder_rules_action_info() {
  return array(
    'rules_geocoder_action_geocode_fields' => array(
      'label' => t('Geocode a field or a combination of fields'),
      'arguments' => array(
        'node' => array(
          'type' => 'node', 
          'label' => t('Content')
        ),
      ),
      'module' => 'Rules Geocoder',
    ),
  );
}

/**
 * Configuration form for the queue filefield action.
 */
function rules_geocoder_action_geocode_fields_form($settings, &$form) {
  $settings += array(
    'sources' => array(),
    'destination' => '',
    'prefix' => '',
    'suffix' => '',
  );

  // Make a list of available text fields.
  // @TODO: Weight/ordering of input fields? Google's geocoder doesn't seem to care if the ordering is crazy though.
  $fields = content_fields();
  $textfield_options = array();
  foreach ($fields as $field) {
    if ($field['type'] == 'text') {
      $textfield_options[$field['field_name']] = $field['widget']['label'];
    }
  }
  $wktfield_options = array();
  foreach ($fields as $field) {
    // @TODO: Add Geo.module support
    if ($field['type'] == 'openlayers_wkt') {
      $wktfield_options[$field['field_name']] = $field['widget']['label'];
    }
  }

  $form['settings']['sources'] = array(
    '#type' => 'select',
    '#title' => t('Source text field(s)'),
    '#multiple' => TRUE,
    '#options' => $textfield_options,
    '#default_value' => $settings['sources'],
  );

  $form['settings']['destination'] = array(
    '#type' => 'select',
    '#title' => t('Destination OpenLayers WKT field'),
    '#options' => $wktfield_options,
    '#default_value' => $settings['destination'],
  );
  $form['settings']['prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Prefix'),
    '#default_value' => $settings['prefix'],
  );
  $form['settings']['suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Suffix'),
    '#default_value' => $settings['suffix'],
  );
}

/**
 * Validate the configuration form.
 */
function rules_geocoder_action_geocode_fields_validate($form, $form_state) {
// @TODO: Anything to validate?
}

/**
 * Help text for the configuration form.
 */
function rules_geocoder_action_geocode_fields_help() {
  return t('Please specify which field(s) should be used as input and the destination OpenLayers WKT field');
}

/**
 * Run the geocoder action.
 */
function rules_geocoder_action_geocode_fields(&$node, $settings) {
  
  $source_fields = $settings['sources'];
  $dest_field = $settings['destination'];
  $prefix = $settings['prefix'];
  $suffix = $settings['suffix'];

  // Check all the necessary prerequisites.
//  $source_field_def = content_fields($source_fields, $node->type); 
// @TODO: Verify that all source fields existing in the current node type
  $dest_field_def = content_fields($dest_field, $node->type);

//  if (!$source_field_def) {
//    drupal_set_message(t('This content could not be converted with FFmpeg Converter since the source field doesn\'t exist in this content type.'), 'error');
//  }
//  elseif (!$dest_field_def) {
//    drupal_set_message(t('This content could not be converted with FFmpeg Converter since the destination field doesn\'t exist in this content type.'), 'error');
//  }
//  else {
  
  $source_strings = array();
  
  foreach ($source_fields as $source_field) {
    foreach ($node->{$source_field} as $field_instance) {
      $source_strings[] = $field_instance['value'];
    }
  }
  $geo_input = ($prefix ? $prefix .' ' : '') . implode(' ', $source_strings) . ($suffix ? ' '. $suffix : '');  
  $geo_output = _rules_geocoder_geocode($geo_input);
  
  foreach ($node->{$dest_field} as $key => $field_instance) {
    // @TODO: Add Geo.module support
    $node->{$dest_field}[$key]['openlayers_wkt'] = $geo_output;
  }
  $logdata = print_r($node, true);
  $logdata .= print_r($geo_output, true);
  $fp = fopen("/Users/henrik/Sites/esny.local/sites/default/files/testdump.txt", "w");
  fwrite($fp, $logdata);
  fclose($fp);
}
