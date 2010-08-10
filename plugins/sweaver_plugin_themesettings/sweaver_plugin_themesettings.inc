<?php
// $Id$

/**
 * @file
 * Theme settings plugin.
 */
class sweaver_plugin_themesettings extends sweaver_plugin {

  /**
   * Frontend form.
   */
  function sweaver_form() {
    $form = array();
    $form['#popups'] = array();
    global $theme_key;

    $form_state = array();
    module_load_include('inc', 'system', 'system.admin');
    $form = system_theme_settings($form_state, $theme_key);

    // Add extra property for theme settings which need to be saved also.
    // Form alters on the theme settings form must be put those keys in this array.
    $form['#sweaver_other_themesettings'] = array();

    // Let other modules alter the form.
    drupal_prepare_form('system_theme_settings', $form, $form_state);

    // Convert all fieldsets to buttons. The content of those fieldsets
    // will be moved into the Sweaver popup.
    $i = 1;
    $weight = -100;
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && $form[$key]['#type'] == 'fieldset') {

        // Skip fieldsets which the user doesn't have access to.
        if (isset($form[$key]['#access']) && $form[$key]['#access'] === FALSE) {
          continue;
        }

        $title = $form[$key]['#title'];

        $form['item_'. $i] = array(
          '#type' => 'markup',
          '#value' => '<div class="popup-link"><a href="#" id="theme-settings-link-'. $i .'">'. check_plain($title) .'</a></div>',
          '#weight' => $weight++,
        );

        $form[$key]['#prefix'] = '<div class="display-none" id="theme-settings-data-'. $i .'">';
        $form[$key]['#suffix'] = '</div>';
        unset($form[$key]['#collapsible']);
        unset($form[$key]['#collapsed']);
        unset($form[$key]['#attributes']);

        // Add to popups.
        $form['#popups'][] = $key;

        $i++;
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
  function sweaver_form_submit($form, &$form_state) {

    $values = $form_state['values'];
    $key = $values['var'];

    if ($values['op'] == t('Reset to defaults')) {
      $this->sweaver_session(t('Your theme settings have been reverted.'));
      variable_del($key);
    }
    if ($values['op'] == t('Save configuration')) {
      $this->sweaver_session(t('Your theme settings have been saved.'));

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

      // Other variables which are set by other form alters.
      // All keys must be put in a $form['#sweaver_other_themesettings'];
      $theme_form_keys = array_merge($theme_form_keys, $form['sweaver_plugin_themesettings']['form']['#sweaver_other_themesettings']);

      // Only save the necessary theme values.
      foreach ($values as $fkey => $fvalue) {
        if (in_array($fkey, $theme_form_keys)) {
          $theme_values[$fkey] = $fvalue;
        }
      }

      // And finally save.
      variable_set($key, $theme_values);

      // Fire other submit functions on the theme settings form.
      $submit_handlers = $form['sweaver_plugin_themesettings']['form']['#submit'];
      foreach ($submit_handlers as $key => $function) {
        if ($function != 'system_theme_settings_submit' && function_exists($function)) {
          $function($form['sweaver_plugin_themesettings']['form'], $form_state);
        }
      }
    }
  }

  /**
   * Frontend css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    drupal_add_js('misc/collapse.js');
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
