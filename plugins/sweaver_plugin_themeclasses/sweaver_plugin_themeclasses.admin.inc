<?php
// $Id$

define('SWEAVER_PLUGIN_THEMECLASSES_DEFAULT', 'skinr');

/**
 * Theme classes list.
 */
function sweaver_themeclasses_list() {
  $form = array();

  $form['sweaver_themeclasses_list'] = array(
    '#type' => 'textarea',
    '#title' => t('Theme groups'),
    '#description' => t('Enter a group which is available in your theme(s) info file. This plugin will try to fetch all classes from it and make them available in a separate tab. Enter one group per line. This is inspired by the skinr way of theming, so we need to find a key called \'class\' to get a list of styles.'),
    '#default_value' => variable_get('sweaver_themeclasses_list', SWEAVER_PLUGIN_THEMECLASSES_DEFAULT),
  );

  return system_settings_form($form);
}