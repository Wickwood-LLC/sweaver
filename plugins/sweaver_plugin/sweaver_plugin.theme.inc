<?php
// $Id$

/**
 * @file
 * Sweaver theming functions.
 */

/**
 * Editor form.
 */
function template_preprocess_sweaver_plugin(&$vars) {
  $form = &$vars['form'];
  $vars['tabs'] = array();
  $vars['tabs_data'] = array();

  ctools_include('plugins');
  ksort($form['#plugins']);
  $plugins = ctools_get_plugins('sweaver', 'plugins');
  foreach ($form['#plugins'] as $key => $plugin) {
    $object = sweaver_get_plugin($plugins, $plugin['name']);
    $object->frontend_form_render($vars, $form, $plugin);
  }

  // Rest of the form.
  $vars['rest_of_form'] = drupal_render($form);
}