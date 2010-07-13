<?php
// $Id$

/**
 * Theme settings plugin.
 */
class sweaver_plugin_themesettings extends sweaver_plugin {

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();
    global $current_theme;

    $form_state = array();
    module_load_include('inc', 'system', 'system.admin');
    $form = system_theme_settings($form_state, $current_theme);

    // Let other modules alter the form.
    drupal_prepare_form('system_theme_settings', $form, $form_state);

    // Convert all fieldsets to collapsible collapsed.
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && $form[$key]['#type'] == 'fieldset') {
        $form[$key]['#collapsible'] = TRUE;
        $form[$key]['#collapsed'] = TRUE;
      }
    }

    return $form;
  }
}