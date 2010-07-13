<?php
// $Id$

/**
 * Themeswitch plugin.
 */
class sweaver_plugin_themeswitch extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {

    $items = array();
    $base = array(
      'load arguments' => array('%map'),
      'access arguments' => array('use editor'),
      'file' => 'plugins/sweaver_plugin_themeswitch/sweaver_plugin_themeswitch.admin.inc',
      'type' => MENU_CALLBACK,
    );

    // Switch theme.
    $items['sweaver_themeswitch'] = $base + array(
      'title' => 'themeswitch',
      'page callback' => 'sweaver_plugin_themeswitch_switch',
      'weight' => $weight++,
    );

    return $items;
  }

  /**
   * Init
   */
  function init() {
    global $custom_theme, $current_theme;
    if (isset($_SESSION['sweaver_plugin_switch_theme'])) {
      $custom_theme = $_SESSION['sweaver_plugin_switch_theme'];
      $current_theme = $_SESSION['sweaver_plugin_switch_theme'];
    }
  }

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();
    $content = '';
    global $current_theme;
    $no_image = drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeswitch/no_screenshot.png';

    $themes = system_theme_data();
    foreach ($themes as $key => $theme) {
      if ($theme->status) {

        $content .= '<div class="sweaver-theme container" style="margin-right: 16px;">';

        if (isset($theme->info['screenshot']) && file_exists($theme->info['screenshot'])) {
          $image_file = $theme->info['screenshot'];
        }
        else {
          $image_file = $no_image;
        }

        if ($current_theme != $key) {
          $switch_description = t('Switch to @theme', array('@theme' => $theme->info['name']));
          $image = theme('image', $image_file, $switch_description, $switch_description, array('width' => '150', 'height' => '90'), FALSE);
          $image = l($image, 'sweaver_themeswitch/'. $key, array('html' => TRUE));
        }
        else {
          $switch_description = t('This theme is currently selected');
          $image = theme('image', $image_file, $switch_description, $switch_description, array('width' => '150', 'height' => '90'), FALSE);
        }

        $content .= $image;
        $content .= '<br />'. check_plain($theme->info['name']);

        $content .= '</div>';
      }
    }

    $form['markup'] = array(
      '#type' => 'markup',
      '#value' => $content,
    );

    return $form;
  }
}