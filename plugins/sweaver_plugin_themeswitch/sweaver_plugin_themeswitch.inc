<?php
// $Id$

/**
 * @file
 * Themeswitch plugin.
 */
class sweaver_plugin_themeswitch extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function sweaver_menu(&$weight, $page_arguments, $base) {

    $items = array();

    // Theme classes groups administration.
    $base['access arguments'] = array('use editor');
    $items['sweaver_themeswitch'] = $base + array(
      'title' => 'Styles',
      'page arguments' => array($page_arguments),
      'type' => MENU_CALLBACK,
      'weight' => $weight++,
    );

    return $items;
  }

  /**
   * Init
   */
  function sweaver_init() {
    global $custom_theme, $theme_key;
    if ($theme_switch = $this->sweaver_session(NULL, 'sweaver_plugin_switch_theme')) {
      $custom_theme = $theme_switch;
      $theme_key = $theme_switch;
    }
  }

  /**
   * Frontend form.
   */
  function sweaver_form() {
    $form = array();
    $content = '';
    global $theme_key;
    $no_image = drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeswitch/no_screenshot.png';

    $themes = db_query("SELECT * FROM {system} WHERE type = 'theme' AND status = '1' ORDER BY name ASC")->fetchAll();
    foreach ($themes as $theme) {
      if ($theme->status) {
        $theme->info = unserialize($theme->info);

        if (isset($theme->info['screenshot']) && file_exists($theme->info['screenshot'])) {
          $image_file = $theme->info['screenshot'];
        }
        else {
          $image_file = $no_image;
        }

        if ($theme_key != $theme->name) {
          $switch_description = t('Switch to @theme', array('@theme' => $theme->info['name']));
          $image = theme('image', array('path' => $image_file, 'alt' => $switch_description, 'title' => $switch_description, 'attributes' => array('width' => '150', 'height' => '90')));
          $image = l($image, 'sweaver_themeswitch/'. $theme->name, array('html' => TRUE, 'query' => drupal_get_destination()));
          $content .= '<div class="selected-image">'. $image;
        }
        else {
          $switch_description = t('This theme is currently selected');
          $image = theme('image', array('path' => $image_file, 'alt' => $switch_description, 'title' => $switch_description, 'attributes' => array('width' => '150', 'height' => '90')));
          $content .= '<div class="selected-image selected-image-default">'. $image;
        }

        $content .= '<br />'. check_plain($theme->info['name']);

        $content .= '</div>';
      }
    }

    $form['markup'] = array(
      '#markup' => $content,
    );

    return $form;
  }

  /**
   * Frontend form submit handler.
   */
  function sweaver_form_submit($form, &$form_state) {
    if ($form_state['clicked_button']['#value'] == t('Save style') && isset($form_state['saved_style_status']) && $form_state['saved_style_status'] == TRUE) {
      $theme_key = $form['#current_theme'];
      variable_set('theme_default', $theme_key);
    }
  }

  /**
   * Frontend themeswitch.
   */
  function menu_callback() {
    $theme = arg(1);
    $theme_default = variable_get('theme_default', 'garland');

    $all_themes = system_theme_data();
    if (isset($all_themes[$theme]) && $all_themes[$theme]->status == 1) {

      if ($theme != $theme_default) {
        $this->sweaver_session(NULL, 'sweaver_plugin_switch_theme', TRUE);
        $this->sweaver_session($theme, 'sweaver_plugin_switch_theme');
      }
      else {
        // Reset session variable because it's the default.
        $this->sweaver_session(NULL, 'sweaver_plugin_switch_theme', TRUE);
      }

      $this->sweaver_session(t('You have switched to @switched_theme', array('@switched_theme' => $all_themes[$theme]->info['name'])));
    }

    // Go back to previous page.
    drupal_goto();
  }
}
