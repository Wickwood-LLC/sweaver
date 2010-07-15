<?php
// $Id$

/**
 * Imagecache plugin
 */
class sweaver_plugin_imagecache extends sweaver_plugin {

  function is_enabled($plugin_name) {
    $imagecache_access = TRUE;
    $plugin_status = variable_get('sweaver_plugin_status_'. $plugin_name, TRUE);
    if (variable_get('sweaver_plugin_imagecache_user_needs_access', FALSE)) {
      if (!user_access('administer imagecache')) {
        $imagecache_access = FALSE;
      }
    }
    if ($plugin_status && $imagecache_access && module_exists('imagecache')) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {

    $items = array();
    $base = array(
      'load arguments' => array('%map'),
      'access arguments' => array('configure sweaver'),
      'file' => 'plugins/sweaver_plugin_imagecache/sweaver_plugin_imagecache.admin.inc',
      'type' => MENU_LOCAL_TASK,
    );

    // Imagecache settings.
    $items['admin/settings/sweaver/imagecache'] = $base + array(
      'title' => 'Imagecache',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_imagecache_settings'),
      'weight' => $weight++,
    );

    return $items;
  }

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();
    $content = '';

    $imagecache_presets = imagecache_presets();
    foreach ($imagecache_presets as $preset_key => $preset) {
      $image_edit = '';
      if (user_access('administer imagecache')) {
        $image_edit = l(t('Edit preset'), 'admin/build/imagecache/'. $preset_key);
      }
      $content .= '<div class="imagecache clear-block"><h3>'. $preset['presetname'] . '<br />' .$image_edit .'</h3>';

      $content .= '<div class="imagecache-description">';
      foreach ($preset['actions'] as $action_key => $action) {
        // skip unknown actions...
        if (!$definition = imagecache_action_definition($action['action'])) {
          continue;
        }

        $content .= '<p>'. t($definition['name']) .': '. theme($action['action'], array('#value' => $action['data'])) .'</p>';
      }
      $content .= '</div></div>';
    }

    $form['sweaver_imagecache_values'] = array(
      '#type' => 'markup',
      '#value' => $content,
    );

    return $form;
  }

  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_imagecache/sweaver_plugin_imagecache.css', 'module');
  }
}