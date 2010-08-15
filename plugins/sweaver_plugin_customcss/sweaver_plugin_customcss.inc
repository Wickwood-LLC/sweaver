<?php
// $Id$

/**
 * @file
 * Custom CSS plugin.
 */
class sweaver_plugin_customcss extends sweaver_plugin {

  /**
   * Frontend form.
   */
  function sweaver_form() {
    $form = array();
    global $theme_key;
    $current_style = $this->sweaver_get_current_style($theme_key);

    $attributes = array();
    $form['sweaver_plugin_custom_css'] = array(
      '#type' => 'textarea',
      '#rows' => 10,
      '#cols' => 80,
      '#resizable' => FALSE,
      '#attributes' => array(
        'class' => array('sweaver-400'),
      ),
      '#default_value' => (isset($current_style->customcss)) ? $current_style->customcss : '',
      '#prefix' => '<div class="form-floater">',
    );
    $form['sweaver_plugin_custom_css_button'] = array(
      '#type' => 'button',
      '#value' => t('Apply'),
      '#suffix' => '</div>',
    );

    return $form;
  }


  /**
   * Frontend form submit handler.
   */
  function sweaver_form_submit($form, &$form_state) {
    if ($form_state['clicked_button']['#value'] == t('Save style') && isset($form_state['saved_style_id'])) {
      db_query("UPDATE {sweaver_style} set customcss = '%s' WHERE thid = %d", $form_state['values']['sweaver_plugin_custom_css'], $form_state['saved_style_id']);
    }
  }

  /**
   * Frontend css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_customcss/sweaver_plugin_customcss.js');
    $inline_settings['sweaver']['invokes'][] = 'sweaver_plugin_customcss';
  }
}