<?php
// $Id$

/**
 * Styles plugin.
 */
class sweaver_plugin_styles extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {

    $items = array();
    $base = array(
      'load arguments' => array('%map'),
      'access arguments' => array('configure sweaver'),
      'file' => 'plugins/sweaver_plugin_styles/sweaver_plugin_styles.admin.inc',
    );

    // Style list.
    $items['admin/settings/sweaver/styles'] = $base + array(
      'title' => 'Styles',
      'page callback' => 'sweaver_styles_list',
      'type' => MENU_LOCAL_TASK,
    );
    $items['admin/settings/sweaver/styles/delete'] = $base + array(
      'title' => 'Delete style',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_styles_delete'),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Frontend form: add styles form.
   */
  function frontend_form() {
    $form = array();
    $form['#style_containers'];
    global $current_theme;

    // Current style.
    $current_style = sweaver_get_current_style($current_theme);

    // Saved styles in database.
    $existing_styles = array(0 => t('Select'));
    $results = db_query("SELECT thid, style FROM {sweaver_style} where theme = '%s'", $current_theme);
    while ($data = db_fetch_array($results)) {
      $existing_styles[$data['thid']] = $data['style'];
    }

    // Containers.
    $form['#style_containers']['save_style_form'] = array(
      'container_name' => t('Save style'),
      'properties' => array(
        'save_existing_style',
        'save_style_name',
        'save_active',
        'save_submit',
      ),
    );
    $form['#style_containers']['load_style_form'] = array(
      'container_name' => t('Load style'),
      'properties' => array(
        'load_thid',
        'load_active',
        'load_submit',
        'load_no_styles',
      ),
    );

    // Save style form.
    if (!empty($existing_styles)) {
      $form['save_existing_style'] = array(
        '#type' => 'select',
        '#title' => t('Save existing style'),
        '#options' => $existing_styles,
        '#default_value' => (isset($current_style->thid)) ? $current_style->thid : 0,
      );
    }
    $form['save_style_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Style name'),
      '#size' => 50,
    );
    $form['save_active'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set as active style'),
      '#default_value' => 1,
    );
    $form['save_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save style'),
    );

    // Load style form.
    if (empty($existing_styles)) {
      $form['load_no_styles'] = array(
        '#type' => 'markup',
        '#value' => t('There are no styles for this theme.')
      );
    }
    else {
      $form['load_thid'] = array(
        '#type' => 'select',
        '#title' => t('Style name'),
        '#options' => $existing_styles,
      );
      $form['load_active'] = array(
        '#type' => 'checkbox',
        '#title' => t('Set as active style'),
        '#default_value' => TRUE,
      );
      $form['load_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Load style'),
      );
    }

    return $form;
  }

  /**
   * Frontend form render.
   */
  function frontend_form_render(&$vars, &$form, $plugin) {

    $name = $plugin['name'];
    $vars['tabs'][$name]['#tab_name'] = $form[$name]['#tab_name'];
    $vars['tabs_data'][$name]['#tab_description'] = $form[$name]['#tab_description'];

    // Containers.
    $output = '';
    foreach ($form[$name]['form']['#style_containers'] as $key => $container_value) {
      $output .= '<div class="container" style="margin-right: 60px;">';
      $output .= '<h2>'. $container_value['container_name'] .'</h2>';
      foreach ($container_value['properties'] as $key => $form_element) {
        if (isset($form[$name]['form'][$form_element])) {
          $output .= drupal_render($form[$name]['form'][$form_element]);
        }
      }
      $output .= '</div>';
    }

    $vars['tabs_data'][$name]['content'] = $output;
  }

  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_styles/sweaver_plugin_styles.css', 'module');
  }

  /**
   * Frontend form submit.
   */
  function frontend_form_submit($form, &$form_state) {
    $_SESSION['sweaver_editor_messages']['style_save'] = '';
    $_SESSION['sweaver_editor_messages']['style_load'] = '';

    // Save style.
    if ($form_state['clicked_button']['#value'] == t('Save style')) {
      $update = array();
      $style = new stdClass;
      $theme_key = $form['#current_theme'];

      if ($form_state['values']['save_active'] == 1) {
        db_query("UPDATE {sweaver_style} set active = 0");
      }

      if (!empty($form_state['values']['save_existing_style'])) {
        $update = array('thid');
        $style->thid = $form_state['values']['save_existing_style'];
      }
      else {
        $style->style = $form_state['values']['save_style_name'];
        if (empty($style->style)) {
          $_SESSION['sweaver_editor_messages']['style_save'] = t('You need to enter a name for your style.');
          return;
        }
      }

      $style->theme = $theme_key;
      $style->css = $form_state['values']['css'];
      $style->timestamp = time();
      $style->active = $form_state['values']['save_active'];
      drupal_write_record('sweaver_style', $style, $update);
      $this->sweaver_export_file($form_state['values']['css_rendered'], $theme_key);
      $form_state['saved_style_id'] = $style->thid;
      $_SESSION['sweaver_editor_messages']['style_save'] = t('The style @style has been saved', array('@style' => $style->style));
    }

    // Load style.
    if ($form_state['clicked_button']['#value'] == t('Load style')) {
      $theme_key = $form['#current_theme'];
      //if ($form_state['values']['load_active'] == 1) {
        db_query("UPDATE {sweaver_style} set active = 0 where active = 1 and theme = '%s'", $theme_key);
        db_query("UPDATE {sweaver_style} set active = 1 where thid = %d and theme = '%s' ", $form_state['values']['load_thid'], $theme_key);
        $_SESSION['sweaver_editor_messages']['style_load'] = t('The style has been loaded');
      //}
    }
  }

  /**
   * Export css to file.
   */
  function sweaver_export_file($css, $theme_key) {
    $css = str_replace('<style type="text/css">', '', $css);
    $css = str_replace('</style>', '', $css);
    $filename = 'sweaver_' . $theme_key . '.css';

    // Create the css within the files folder.
    $path = file_create_path('sweaver');
    file_check_directory($path, FILE_CREATE_DIRECTORY);
    $file = $path .'/'. $filename;
    if (!$fp = fopen($file, 'w')) {
      $_SESSION['sweaver_editor_messages'][] = t('The css file could not be created.');
      return FALSE;
    }
    fwrite($fp, $css);
    fclose($fp);
  }
}
