<?php
// $Id$

/**
 * Custom CSS plugin.
 */
class sweaver_plugin_customcss extends sweaver_plugin {

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();
    global $current_theme;
    $current_style = sweaver_get_current_style($current_theme);

    $attributes = array();
    $form['sweaver_plugin_custom_css'] = array(
      '#type' => 'textarea',
      '#rows' => 10,
      '#cols' => 80,
      '#resizable' => FALSE,
      '#attributes' => array(
        'class' => 'sweaver_plugin_custom_css',
      ),
      '#default_value' => (isset($current_style->customcss)) ? $current_style->customcss : '',
    );
    $form['sweaver_plugin_custom_css_button'] = array(
      '#type' => 'button',
      '#value' => t('Apply'),
      '#attributes' => array(
        'class' => 'sweaver_plugin_custom_css_button',
      ),
    );

    return $form;
  }

  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_customcss/sweaver_plugin_customcss.css', 'module');
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_customcss/sweaver_plugin_customcss.js', 'module');
    $inline_settings['sweaver']['invokes'][] = 'sweaver_plugin_customcss';
  }

  /**
   * Frontend form submit handler.
   */
  function frontend_form_submit($form, &$form_state) {
    if ($form_state['clicked_button']['#value'] == t('Save style') && isset($form_state['saved_style_id'])) {
      db_query("UPDATE {sweaver_style} set customcss = '%s' WHERE thid = %d", $form_state['values']['sweaver_plugin_custom_css'], $form_state['saved_style_id']);
    }
  }
}