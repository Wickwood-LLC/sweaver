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
    global $theme_key;

    $form_state = array();
    module_load_include('inc', 'system', 'system.admin');
    $form = system_theme_settings($form_state, $theme_key);

    // Let other modules alter the form.
    drupal_prepare_form('system_theme_settings', $form, $form_state);

    // Convert all fieldsets to collapsible collapsed.
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && $form[$key]['#type'] == 'fieldset') {
        $form[$key]['#collapsible'] = TRUE;
        $form[$key]['#collapsed'] = TRUE;
        $form[$key]['#attributes']['class'] = 'create-tab';
      }
    }

    // Add clear block to buttons.
    $form['buttons']['#type'] = 'item';
    $form['buttons']['#prefix'] = '<div class="clear-block"></div>';

    return $form;
  }

  /**
   * Frontend form submit.
   */
  function frontend_form_submit($form, &$form_state) {
    $_SESSION['sweaver_editor_messages']['theme_save'] = '';

    $values = $form_state['values'];
    $key = $values['var'];

   if ($values['op'] == t('Reset to defaults')) {
      $_SESSION['sweaver_editor_messages']['theme_save'] = t('Your theme settings have been reverted.');
      variable_del($key);
    }
    if ($values['op'] == t('Save configuration')) {
      $_SESSION['sweaver_editor_messages']['theme_save'] = t('Your theme settings have been saved.');

      // General theme settings.
      $theme_values = array();
      $theme_settings_form = $form['sweaver_plugin_themesettings']['form']['theme_settings'];
      $theme_form_keys = element_children($theme_settings_form);

      // Logo and favicon
      if (isset($form['sweaver_plugin_themesettings']['form']['logo'])) {
        $theme_form_keys = array_merge($theme_form_keys, array('default_logo', 'logo_path', 'logo_upload'));
      }
      if (isset($form['sweaver_plugin_themesettings']['form']['favicon'])) {
        $theme_form_keys = array_merge($theme_form_keys, array('default_favicon', 'favicon_path', 'favicon_upload'));
      }

      // Theme specific settings.
      if (isset($form['sweaver_plugin_themesettings']['form']['theme_specific'])) {
        $theme_specific_keys = array();
        $fapi_types = array('select', 'checkbox', 'checkboxes', 'textfield', 'textarea', 'value', 'hidden', 'radio', 'weight', 'radios');
        $this->get_theme_specific_keys($form['sweaver_plugin_themesettings']['form']['theme_specific'], $theme_specific_keys, $fapi_types);
        $theme_form_keys = array_merge($theme_form_keys, $theme_specific_keys);
      }

      // Only save necessary theme values.
      foreach ($values as $fkey => $fvalue) {
        if (in_array($fkey, $theme_form_keys)) {
          $theme_values[$fkey] = $fvalue;
        }
      }

      // And finally save.
      variable_set($key, $theme_values);

      // TODO Fire other submit functions on the theme settings form.
    }
  }

  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    //drupal_add_js('misc/collapse.js');
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themesettings/sweaver_plugin_themesettings.js', 'module');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themesettings/sweaver_plugin_themesettings.css', 'theme');
  }

  /**
   * Get theme specific keys.
   */
  function get_theme_specific_keys($theme_specific_form, &$theme_specific_keys, $fapi_types) {
    $children = element_children($theme_specific_form);
    foreach ($children as $element) {
      if (in_array($theme_specific_form[$element]['#type'], $fapi_types)) {
        $theme_specific_keys[] = $element;
      }
      else {
        $this->get_theme_specific_keys($theme_specific_form[$element], $theme_specific_keys, $fapi_types);
      }
    }
  }
}
