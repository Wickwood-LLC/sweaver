<?php
// $Id$

/**
 * Base class for a sweaver plugin.
 */

class sweaver_plugin {

  function is_enabled($plugin_name) {
    return variable_get('sweaver_plugin_status_'. $plugin_name, TRUE);
  }

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {
    return array();
  }

  /**
   * Theme registry.
   */
  function theme_registry() {
    $theme_functions = array();

    $plugin_path = drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin';
    $theme_functions = array(
      'sweaver_plugin' => array(
        'template' => 'sweaver-plugin',
        'file' => 'sweaver_plugin.theme.inc',
        'path' => $plugin_path,
        'arguments' => array('form' => NULL),
      ),
    );

    return $theme_functions;
  }

  /**
   * Init function.
   */
  function init() {
    global $current_theme;
    // Check which theme we are in. Unfortunately we cannot use global $theme_key here yet.
    $current_theme = variable_get('theme_default','garland');
  }

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();
    global $current_theme;

    $current_style = sweaver_get_current_style($current_theme);
    $form['css'] = array(
      '#type' => 'hidden',
      '#default_value' => isset($current_style->css) ? $current_style->css : '',
    );
    $form['css_rendered'] = array(
      '#type' => 'hidden',
      '#default_value' => '',
    );
    return $form;
  }

  /**
   * Frontend form render.
   */
  function frontend_form_render(&$vars, &$form, $plugin) {
    $name = $plugin['name'];
    if (isset($form[$name])) {
      if (isset($form[$name]['#tab_name'])) {
        $vars['tabs'][$name]['#tab_name'] = $form[$name]['#tab_name'];
      }
      $vars['tabs_data'][$name]['#tab_description'] = $form[$name]['#tab_description'];
      $vars['tabs_data'][$name]['content'] = drupal_render($form[$name]['form']);
    }
  }

  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.js', 'module');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.css', 'module');
  }

  /**
   * Frontend form submit handler.
   */
  function frontend_form_submit($form, &$form_state) {}
}
