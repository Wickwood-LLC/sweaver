<?php
// $Id$

/**
 * @file
 * Administrative functions for Sweaver.
 */

/**
 * Settings form.
 */
function sweaver_settings($form_state) {
  $form = array();

  $form['sweaver_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable sweaver editor'),
    '#default_value' => variable_get('sweaver_enabled', TRUE),
    '#description' => t('Enable the editor at the bottom.'),
  );

  $form['sweaver_paths'] = array(
    '#type' => 'textarea',
    '#title' => t('Exclude paths'),
    '#default_value' => variable_get('sweaver_paths', SWEAVER_PATHS_EXCLUDE),
    '#description' => t('Exclude the editor on some pages, usually administration pages. Enter one page per line as Drupal paths. The \'*\' character is a wildcard. &lt;front&gt; is the front page.'),
  );

  $form['sweaver_selectors_exclude'] = array(
    '#type' => 'textarea',
    '#title' => t('Exclude selectors'),
    '#default_value' => variable_get('sweaver_selectors_exclude', SWEAVER_SELECTORS_EXCLUDE),
    '#description' => t('Define on which items the style editor should not be applicable. Enter one selector per line.'),
  );

  $form['sweaver_classes_exclude'] = array(
    '#type' => 'textarea',
    '#title' => t('Exclude classes'),
    '#default_value' => variable_get('sweaver_classes_exclude', SWEAVER_CLASSES_EXCLUDE),
    '#description' => t('Define which classes should never be displayed in the css path. Enter one selector per line.'),
  );

  return system_settings_form($form);
}

/**
 * sweaver objects list.
 */
function sweaver_objects_list($object_type) {

  $output = l(t('Add new @sweaver_object', array('@sweaver_object' => $object_type)), 'admin/settings/sweaver/editor/add/'. $object_type);
  drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_editor/css/sweaver_plugin_editor.admin.css');

  $objects = sweaver_object_load(NULL, $object_type, 'all');
  if (!empty($objects)) {
    $rows = array();
    $rows_enabled = array();
    $rows_disabled = array();
    ksort($objects);

    foreach ($objects as $key => $object) {

      $object_name = check_plain($object->name);
      $object_export_type = $object->export_type;

      $row = array();
      $row[] = $object_name;
      $row[] = check_plain($object->description);
      $row[] = $object->type;

      // Operations.
      $operations = array();
      $operations['edit'] = l(t('Edit'), 'admin/settings/sweaver/editor/edit/'. $object_type .'/'. $object_name);

      if ($object_export_type != 2) {
        $operations['export'] = l(t('Export'), 'admin/settings/sweaver/editor/export/'. $object_type .'/'. $object_name);
      }

      // Delete - revert.
      if ($object_export_type == 1) {
        $operations['delete'] = l(t('Delete'), 'admin/settings/sweaver/editor/delete/'. $object_type .'/'. $object_name);
      }
      elseif ($object_export_type == 3) {
        $operations['delete'] = l(t('Revert'), 'admin/settings/sweaver/editor/revert/'. $object_type .'/'. $object_name);
      }

      // Disable - enable.
      $row_enabled = TRUE;
      if ($object_export_type == 2) {
        if ($object->disabled) {
          $row_enabled = FALSE;
          unset($operations['edit']);
          unset($operations['export']);
          unset($operations['delete']);
          $operations['status'] = l(t('Enable'), 'admin/settings/sweaver/editor/enable/'. $object_type .'/'. $object_name);
        }
        else {
          $operations['status'] = l(t('Disable'), 'admin/settings/sweaver/editor/disable/'. $object_type .'/'. $object_name);
        }
      }
      $row[] = implode(' - ', $operations);

      // Add selector to enabled or disabled row.
      if ($row_enabled) {
        $rows_enabled[] = $row;
      }
      else {
        $rows_disabled[] = array('data' => $row, 'class' => 'disabled');
      }
    }

    // Output table.
    $header = array(
      t('Name'),
      t('Description'),
      t('Status'),
      t('Operations'),
    );

    // Add an extra empty row to split enabled and disabled.
    if (!empty($rows_disabled)) {
      $empty_row = array(array('data' => '&nbsp;', 'colspan' => '6', 'class' => 'empty'));
      array_unshift($rows_disabled, $empty_row);
    }

    $rows = array_merge($rows_enabled, $rows_disabled);
    $output .= theme('table', $header, $rows, array('id' => 'objects'));
  }
  else {
    $output .= t('<p>No @object_type available.</p>', array('@object_type' => $object_type));
  }

  return $output;
}

/**
 * Object form.
 */
function sweaver_object_form($form_state, $object_type, $object = NULL) {
  $form = array();
  $function = 'sweaver_'. $object_type .'_form';
  return $function($form, $object);
}

/**
 * Object new/edit validate callback.
 */
function sweaver_object_form_validate(&$form, &$form_state) {
  $name = $form_state['values']['name'];
  $check_name = $form_state['values']['check_name'];
  $object = sweaver_object_load($name, $form['#object_type']);

  if (isset($object->name) && $check_name) {
    form_set_error('name', t('The machine-readable name already exists'));
  }

  if (!preg_match('!^[a-z0-9_-]+$!', $name)) {
    form_set_error('name', t('The machine-readable name must contain only lowercase letters, underscores or hyphens.'));
  }
}

/**
 * Selector new/edit form.
 */
function sweaver_selector_form(&$form, $selector = NULL) {

  if (empty($selector)) {
    $selector = new stdClass;
    $selector->name = '';
    $selector->description = '';
    $selector->selector_selector = '';
    $selector->selector_highlight = FALSE;
    $selector->export_type = 1;
  }

  if (!isset($selector->oid)) {
    $selector->oid = NULL;
  }

  $form['#object_type'] = 'selector';

  $form['check_name'] = array(
    '#type' => 'value',
    '#value' => ($selector->export_type == 1 && !isset($selector->oid)) ? TRUE : FALSE,
  );

  $form['oid'] = array(
    '#type' => 'value',
    '#value' => $selector->oid,
  );

  $form['name'] = array(
    '#title' => t('Machine name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $selector->name,
    '#description' => t('The machine-readable name of this selector. This name must contain only lowercase letters, numbers, underscores or hyphens and must be unique.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $selector->description,
    '#description' => t('Description of this selector which will be used in the editor.'),
  );
  $form['selector_selector'] = array(
    '#title' => t('Selector'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $selector->selector_selector,
    '#description' => t('The CSS selector which the editor will act on e.g. a.link'),
  );
  $form['selector_highlight'] = array(
    '#title' => t('Highlight'),
    '#type' => 'checkbox',
    '#default_value' => $selector->selector_highlight,
    '#description' => t('Should this selector be highlighted in the path ?'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('sweaver_selector_form_submit'),
  );

  return $form;
}

/**
 * Selector new/edit submit callback.
 */
function sweaver_selector_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];

  // Build selector.
  $selector = new stdClass;
  $selector->oid = $values['oid'];
  $selector->name = $values['name'];
  $selector->description = $values['description'];
  $selector->selector_selector = $values['selector_selector'];
  $selector->selector_highlight = $values['selector_highlight'];

  // Save selector.
  $update = (isset($selector->oid) && is_numeric($selector->oid)) ? array('oid') : array();
  drupal_write_record('sweaver_selector', $selector, $update);
  cache_clear_all('sweaver', 'cache');

  // Message.
  $action = empty($update) ? 'added' : 'updated';
  $message = t('Selector %selector has been @action.', array('%selector' => $selector->name, '@action' => $action));

  // Message and redirect.
  drupal_set_message($message);
  $form_state['redirect'] = 'admin/settings/sweaver/editor/selectors';
}

/**
 * Property new/edit form.
 */
function sweaver_property_form(&$form, $property = NULL) {

  if (empty($property)) {
    $property = new stdClass;
    $property->name = '';
    $property->property_parent = '';
    $property->description = '';
    $property->property_type = '';
    $property->property_prefix = '';
    $property->property_suffix = '';
    $property->property_slider_min = 1;
    $property->property_slider_max = 72;
    $property->property_options = array();
    $property->export_type = 1;
  }

  if (!isset($property->oid)) {
    $property->oid = NULL;
  }

  // Parents.
  $parents = array('' => '');
  $all_properties = sweaver_object_load(NULL, 'property', 'all');
  foreach ($all_properties as $key => $prop) {
    if ($prop->property_type == 'parent') {
      $parents[$key] = $prop->description;
    }
  }

  $form['#object_type'] = 'property';

  $form['check_name'] = array(
    '#type' => 'value',
    '#value' => ($property->export_type == 1 && !isset($property->oid)) ? TRUE : FALSE,
  );

  $form['oid'] = array(
    '#type' => 'value',
    '#value' => $property->oid,
  );

  $form['name'] = array(
    '#title' => t('Machine name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $property->name,
    '#description' => t('The machine-readable name of this property. This name must contain only lowercase letters, numbers, underscores or hyphens and must be unique.'),
  );
  $form['property_parent'] = array(
    '#title' => t('Parent'),
    '#type' => 'select',
    '#options' => $parents,
    '#default_value' => $property->property_parent,
    '#description' => t('The parent of this property. Eg, padding-top should have a parent called padding.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $property->description,
    '#description' => t('Description of this property which will be used in the editor.'),
  );
  $form['property_prefix'] = array(
    '#title' => t('Prefix'),
    '#type' => 'textfield',
    '#default_value' => $property->property_prefix,
    '#description' => t('Prefix of this property.'),
  );
  $form['property_suffix'] = array(
    '#title' => t('Suffix'),
    '#type' => 'textfield',
    '#default_value' => $property->property_suffix,
    '#description' => t('Suffix of this property.'),
  );

  $types = array(
    'parent' => t('Parent'),
    'slider' => t('Slider'),
    'select' => t('Select'),
    'color' => t('Color'),
    'image' => t('Image'),
  );
  $form['property_type'] = array(
    '#title' => t('Type'),
    '#type' => 'select',
    '#options' => $types,
    '#default_value' => $property->property_type,
    '#description' => t('Selection type of this property. A parent can have children underneath, grouping other stuff together.'),
  );
  $form['property_slider_min'] = array(
    '#title' => t('Slider minimum'),
    '#type' => 'textfield',
    '#default_value' => $property->property_slider_min,
    '#description' => t('Minimum value for the slider, only applicable for the slider.'),
  );
  $form['property_slider_max'] = array(
    '#title' => t('Slider maximum'),
    '#type' => 'textfield',
    '#default_value' => $property->property_slider_max,
    '#description' => t('Maximum value for the slider, only applicable for the slider.'),
  );

  $property_options = '';
  sweaver_export_check_serialized_keys($property);
  if (is_array($property->property_options)) {
    foreach ($property->property_options as $key => $value) {
      $property_options .= $key .'|'. $value ."\n";
    }
  }
  $form['property_options'] = array(
    '#title' => t('Options'),
    '#type' => 'textarea',
    '#default_value' => $property_options,
    '#description' => t('Options for this property. Enter options per line and separate key and value by |.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('sweaver_property_form_submit'),
  );

  return $form;
}

/**
 * Property new/edit submit callback.
 */
function sweaver_property_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];

  // Build selector.
  $property = new stdClass;
  $property->oid = $values['oid'];
  $property->name = $values['name'];
  $property->description = $values['description'];
  $property->property_parent = $values['property_parent'];
  $property->property_type = $values['property_type'];
  $property->property_prefix = $values['property_prefix'];
  $property->property_suffix = $values['property_suffix'];
  $property->property_slider_min = $values['property_slider_min'];
  $property->property_slider_max = $values['property_slider_max'];
  $options = array();
  $property_options = explode("\n", trim($values['property_options']));
  foreach ($property_options as $key => $line) {
    $line_explode = explode("|", $line);
    if (count($line_explode) == 2) {
      $options[$line_explode[0]] = trim($line_explode[1]);
    }
  }
  $property->property_options = serialize($options);

  // Save property.
  $update = (isset($property->oid) && is_numeric($property->oid)) ? array('oid') : array();
  drupal_write_record('sweaver_property', $property, $update);
  cache_clear_all('sweaver', 'cache');

  // Message.
  $action = empty($update) ? 'added' : 'updated';
  $message = t('Property %property has been @action.', array('%property' => $property->name, '@action' => $action));

  // Message and redirect.
  drupal_set_message($message);
  $form_state['redirect'] = 'admin/settings/sweaver/editor/properties';
}

/**
 * Type new/edit form.
 */
function sweaver_type_form(&$form, $type = NULL) {

  if (empty($type)) {
    $type = new stdClass;
    $type->name = '';
    $type->description = '';
    $type->type_options = array();
    $type->export_type = 1;
  }

  if (!isset($type->oid)) {
    $type->oid = NULL;
  }

  $form['#object_type'] = 'type';

  $form['check_name'] = array(
    '#type' => 'value',
    '#value' => ($type->export_type == 1 && !isset($type->oid)) ? TRUE : FALSE,
  );

  $form['oid'] = array(
    '#type' => 'value',
    '#value' => $type->oid,
  );

  $form['name'] = array(
    '#title' => t('Machine name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $type->name,
    '#description' => t('The machine-readable name of this type. This name must contain only lowercase letters, numbers, underscores or hyphens and must be unique.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $type->description,
    '#description' => t('Description of this type only to be used for administration forms.'),
  );

  $type_options = sweaver_ctools_object_list('property', -1);
  sweaver_export_check_serialized_keys($type);
  $form['type_options'] = array(
    '#title' => t('Options'),
    '#type' => 'checkboxes',
    '#options' => $type_options,
    '#default_value' => $type->type_options,
    '#description' => t('Options for this type.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('sweaver_type_form_submit'),
  );

  return $form;
}

/**
 * Property new/edit submit callback.
 */
function sweaver_type_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];

  // Build selector.
  $type = new stdClass;
  $type->oid = $values['oid'];
  $type->name = $values['name'];
  $type->description = $values['description'];
  $options = $values['type_options'];
  foreach ($options as $key => $value) {
    if ($key !== $value) {
      unset($options[$key]);
    }
  }
  $type->type_options = serialize($options);

  // Save property.
  $update = (isset($type->oid) && is_numeric($type->oid)) ? array('oid') : array();
  drupal_write_record('sweaver_type', $type, $update);
  cache_clear_all('sweaver', 'cache');

  // Message.
  $action = empty($update) ? 'added' : 'updated';
  $message = t('Type %type has been @action.', array('%type' => $type->name, '@action' => $action));

  // Message and redirect.
  drupal_set_message($message);
  $form_state['redirect'] = 'admin/settings/sweaver/editor/types';
}

/**
 * Export an object.
 */
function sweaver_object_export($object_type, $object) {
  drupal_set_title(check_plain($object->description));
  $code = sweaver_export_sweaver_object($object, $object_type);
  return drupal_get_form('ctools_export_form', $code, check_plain($object->description));
}

/**
 * Delete object form.
 */
function sweaver_object_delete(&$form_state, $redirect, $object_type, $object, $action, $action_message) {
  if ($object->export_type != 2) {
    $form['#object'] = $object;
    $form['#object_type'] = $object_type;
    $form['#object_redirect'] = $redirect;
    $form['#action_message'] = $action_message;
    return confirm_form($form, t('Are you sure you want to @action @type %object?', array('@action' => $action, '@type' => $object_type , '%object' => $object->name)), 'admin/settings/sweaver/'. $redirect);
  }
  else {
    drupal_goto('admin/settings/sweaver/editor/'. $redirect);
  }
}

/**
 * Delete object submit callback.
 */
function sweaver_object_delete_submit(&$form, &$form_state) {
  cache_clear_all('sweaver', 'cache');
  $object = $form['#object'];
  $redirect = $form['#object_redirect'];
  $object_type = $form['#object_type'];
  $action_message = $form['#action_message'];
  db_query("DELETE FROM {sweaver_%s} WHERE oid = %d", $object_type, $object->oid);
  drupal_set_message(t('Selector %selector has been @action_message.', array('@action_message' => $action_message, '%selector' => $object->name)));
  $form_state['redirect'] = 'admin/settings/sweaver/editor/'. $redirect;
}

/**
 * Enable or disable an object.
 */
function sweaver_object_status($redirect, $object, $status) {
  ctools_include('export');
  ctools_export_set_object_status($object, $status);
  cache_clear_all('sweaver', 'cache');
  drupal_goto('admin/settings/sweaver/editor/'. $redirect);
}

/**
 * Editor form.
 */
function sweaver_plugin_editor_config_editor(&$form_state) {
  $form = array();
  $form['#tree'] = TRUE;
  $form['#properties_tree'] = array();
  $form['#properties_region'] = array();
  $sweaver = sweaver_get_plugin('sweaver_plugin');
  $form['#containers'] = $sweaver->sweaver_containers();
  $sweaver_editor_form_configuration = variable_get('sweaver_editor_form_configuration', array());

  $order = array();
  foreach ($sweaver_editor_form_configuration as $container => $settings) {

    $form['container_form_'. $container] = array(
      '#type' => 'textfield',
      '#default_value' => $settings['title'],
      '#size' => 15,
      '#attributes' => array(
        'class' => 'container-label-edit'
      ),
    );

    foreach ($settings['properties'] as $weight => $field) {
      $order[$field]['weight'] = $weight;
      $order[$field]['container'] = $container;
    }
  }

  $new_weight = 100;
  $properties = sweaver_object_load(NULL, 'property', 'enabled');
  foreach ($properties as $key => $property) {

    if (!empty($property->property_parent)) {
      continue;
    }

    $container = isset($order[$key]) ? $order[$key]['container'] : 'one';
    $weight = isset($order[$key]) ? $order[$key]['weight'] : $new_weight;

    $form['#properties_tree'][$key] = $key;
    $form['#properties_region'][$container][$weight] = $key;

    $form[$key]['name'] = array(
      '#type' => 'markup',
      '#value' => $property->description,
    );
    $form[$key]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 50,
      '#attributes' => array(
        'class' => 'property-weight property-weight-'. $container
      ),
      '#default_value' => $weight,
    );
    $form[$key]['container'] = array(
      '#type' => 'select',
      '#options' => $sweaver->sweaver_containers(),
      '#attributes' => array(
        'class' => 'property-container-select property-container-'. $container
      ),
      '#default_value' => $container,
    );

    $new_weight++;
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Editor form submit callback.
 */
function sweaver_plugin_editor_config_editor_submit(&$form, &$form_state) {

  $values = $form_state['values'];
  $editor_form_configuration = array();

  // Properties.
  foreach ($form['#properties_tree'] as $property_key) {
    $property = $values[$property_key];
    $editor_form_configuration[$property['container']]['properties'][$property['weight']] = $property_key;
  }

  // Containers title and properties order.
  foreach ($form['#containers'] as $container_key => $container) {
    $editor_form_configuration[$container_key]['title'] = $values['container_form_'. $container_key];
    if (isset($editor_form_configuration[$container_key]['properties'])) {
      ksort($editor_form_configuration[$container_key]['properties']);
    }
    else {
      $editor_form_configuration[$container_key]['properties'] = array();
    }
  }

  drupal_set_message(t('Your settings have been saved.'));
  variable_set('sweaver_editor_form_configuration', $editor_form_configuration);
}
