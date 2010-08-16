<?php
// $Id$

/**
 * @file
 * Sweaver theming functions.
 */

/**
 * Plugins drag and drop configuration screen.
 */
function template_preprocess_sweaver_plugin_config_plugins(&$vars) {
  $form = &$vars['form'];
  $rows = array();

  ksort($form['#plugins']);
  foreach ($form['#plugins'] as $key => $plugin) {
    $row = new stdClass;
    $row->name = drupal_render($form[$plugin]['name']);
    $row->status = drupal_render($form[$plugin]['status']);
    $row->weight = drupal_render($form[$plugin]['weight']);
    $rows[] = $row;
  }

  drupal_add_js('misc/tableheader.js');
  drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.admin.css');
  drupal_add_tabledrag('plugins-configuration', 'order', 'sibling', 'plugin-weight', NULL, NULL, TRUE);

  $vars['rows'] = $rows;
  $vars['submit'] = drupal_render_children($form);
}
