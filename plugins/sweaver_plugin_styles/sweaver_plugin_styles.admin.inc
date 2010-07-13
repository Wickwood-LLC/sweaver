<?php
// $Id$

/**
 * @file
 * Styles administration.
 */

/**
 * Menu callback, styles list.
 */
function sweaver_styles_list() {

  $rows = array();
  $result = db_query("SELECT * FROM {sweaver_style} ORDER BY style ASC, theme ASC, active DESC");
  while ($style = db_fetch_object($result)) {
    $row = array();
    $row[] = check_plain($style->style);
    $row[] = check_plain($style->theme);
    $row[] = ($style->active) ? t('Active') : t('Inactive');
    $operations = l(t('Delete'), 'admin/settings/sweaver/styles/delete/'. $style->thid);
    $row[] = $operations;
    $rows[] = $row;
  }

  if (empty($rows)) {
    return '<p>'. t('No styles found.') .'</p>';
  }
  else {

    $header = array(
      t('Style'),
      t('Theme'),
      t('Status'),
      t('Operations'),
    );

    // Settings.
    $output = drupal_get_form('sweaver_plugin_styles_settings');

    // Styles list.
    $output .= theme('table', $header, $rows);

  }

  return $output;
}

/**
 * Styles settings form.
 */
function sweaver_plugin_styles_settings(&$form_state) {
  $form = array();

  $form['sweaver_load_style_when_editor_inactive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Load style'),
    '#description' => t('Load active style for the theme when the editor is inactive. You can also add a line to the info file of your theme like this: stylesheets[all][] = /path/to/cssfile after you copied the file to your theme path. If you do that, you can ignore this setting completely and even disable this module after you have themed your website.'),
    '#default_value' => variable_get('sweaver_load_style_when_editor_inactive', TRUE),
  );

  return system_settings_form($form);
}

/**
 * Menu callback, delete style.
 */
function sweaver_styles_delete(&$form_state, $thid = NULL) {
  $style = db_fetch_object(db_query('SELECT * FROM {sweaver_style} WHERE thid = %d', $thid));
  if ($style->thid) {
    $form['#style'] = $style;
    return confirm_form($form, t('Are you sure you want to delete style %style?', array('%style' => $style->style)), 'admin/settings/sweaver/styles');
  }
  else {
    drupal_set_message(t('Style not found'));
    drupal_goto('admin/settings/sweaver/styles');
  }
}

/**
 * Submit callback, delete style.
 */
function sweaver_styles_delete_submit($form, &$form_state) {
  db_query("DELETE FROM {sweaver_style} WHERE thid = %d", $form['#style']->thid);
  drupal_set_message(t('Style %style has been removed', array('%style' => $form['#style']->style)));
  $form_state['redirect'] = 'admin/settings/sweaver/styles';
}
