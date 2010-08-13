<?php
// $Id$

/**
 * Sweaver imagecache plugin settings.
 */
function sweaver_imagecache_settings(&$form_state) {
  $form = array();

  $form['sweaver_plugin_imagecache_user_needs_access'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only show tab with imagecache access'),
    '#description' => t('Toggle this checkbox if the user also must have access to imagecache to view the plugin tab.'),
    '#default_value' => variable_get('sweaver_plugin_imagecache_user_needs_access', FALSE),
  );

  // Use imagecache on images ?
  ctools_include('plugins');
  $plugins = ctools_get_plugins('sweaver', 'plugins');
  $object = sweaver_get_plugin($plugins, 'sweaver_plugin_images');

  if ($object->is_enabled('sweaver_plugin_images')) {
    $form['sweaver_plugin_imagecache_alter_images'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow to see imagecached version of images'),
      '#description' => t('Toggle this checkbox if you want to create imagecached versions of the images uploaded by the Images plugin.'),
      '#default_value' => variable_get('sweaver_plugin_imagecache_alter_images', FALSE),
    );
  }
  else {
    $form['images_plugin_disabled'] = array(
      '#type' => 'markup',
      '#value' => t('If you enable the images plugin, all images will be available also as their imagecache plugin variant.'),
    );
  }

  return system_settings_form($form);
}