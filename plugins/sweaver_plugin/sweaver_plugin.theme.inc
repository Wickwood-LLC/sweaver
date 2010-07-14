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
  $active_tab = '';
  $vars['tabs'] = array();
  $vars['tabs_data'] = array();

  ctools_include('plugins');
  ksort($form['#plugins']);
  $plugins = ctools_get_plugins('sweaver', 'plugins');

  $i = 0;
  foreach ($form['#plugins'] as $key => $plugin) {
    if ($i == 0) {
      $active_tab = $plugin['name'];
    }
    $object = sweaver_get_plugin($plugins, $plugin['name']);
    $object->frontend_form_render($vars, $form, $plugin);
    $i++;
  }

  // Active tab
  if (isset($_COOKIE['sweaver_active_tab']) && !empty($_COOKIE['sweaver_active_tab'])) {
    $active_tab = str_replace('tab-', '', $_COOKIE['sweaver_active_tab']);
  }
  $vars['active_tab'] = $active_tab;

  // Rest of the form.
  $vars['rest_of_form'] = drupal_render($form);
}