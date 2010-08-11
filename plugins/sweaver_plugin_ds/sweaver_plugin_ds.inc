<?php
// $Id$

/**
 * @file
 * Display Suite plugin.
 */

class sweaver_plugin_ds extends sweaver_plugin {

  function sweaver_enabled($plugin_name) {
    return variable_get('sweaver_plugin_status_'. $plugin_name, TRUE) && module_exists('ds');
  }

  /**
   * Sweaver dependencies.
   */
  function sweaver_dependencies() {
    return array('ds');
  }

  /**
   * Frontend css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_ds/sweaver_plugin_ds.js', 'module');
  }
}
