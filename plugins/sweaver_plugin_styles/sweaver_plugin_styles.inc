<?php
// $Id$

/**
 * @file
 * Styles plugin.
 */
class sweaver_plugin_styles extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function sweaver_menu(&$weight, $page_arguments, $base) {

    $items = array();
    $page_arguments = array(
      'plugin' => 'sweaver_plugin_styles',
    );

    // Theme classes groups administration.
    $items['admin/config/user-interface/sweaver/styles'] = $base + array(
      'title' => 'Styles',
      'page arguments' => array($page_arguments),
      'type' => MENU_LOCAL_TASK,
      'weight' => $weight++,
    );

    // Delete a style.
    $page_arguments['callback_method'] = 'style_confirm_form_delete';
    $items['admin/config/user-interface/sweaver/styles/delete'] = array(
      'title' => 'Delete style',
      'page arguments' => array($page_arguments),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Frontend form: add styles form.
   */
  function sweaver_form() {
    $form = array();
    $form['#style_containers'] = array();
    global $theme_key;

    // Current style.
    $current_style = $this->sweaver_get_current_style($theme_key);

    // Saved styles in database.
    $existing_styles = FALSE;
    $existing_styles_options = array(0 => t('Select'));
    $results = db_query("SELECT thid, style FROM {sweaver_style} where theme = :theme", array(':theme' => $theme_key))->fetch();
    foreach ($results as $data) {
      $existing_styles_options[$data->thid] = $data->style;
    }
    if (count($existing_styles_options) > 1) {
      $existing_styles = TRUE;
    }

    // Add save to containers.
    $form['#style_containers']['save_style_form'] = array(
      'container_name' => t('Save style'),
      'properties' => array(
        'save_existing_style',
        'save_style_name',
        'save_active',
        'save_submit',
      ),
    );

    // Save style form.
    if ($existing_styles) {
      $form['save_existing_style'] = array(
        '#type' => 'select',
        '#title' => t('Save existing style'),
        '#options' => $existing_styles_options,
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
      '#default_value' => isset($current_style->active) ? $current_style->active : 0,
    );

    $form['save_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save style'),
    );

    // Load /delete style form.
    if ($existing_styles) {

      // Add load and delete to containers.
      $form['#style_containers']['load_style_form'] = array(
        'container_name' => t('Load style'),
        'properties' => array(
          'load_thid',
          'load_active',
          'load_submit',
        ),
      );
      $form['#style_containers']['delete_style_form'] = array(
        'container_name' => t('Delete style'),
        'properties' => array(
          'delete_thid',
          'delete_submit',
        ),
      );

      $form['load_thid'] = array(
        '#type' => 'select',
        '#title' => t('Style'),
        '#options' => $existing_styles_options,
        '#description' => t('Selecting nothing will set all styles for this theme to inactive.'),
      );
      $form['load_active'] = array(
        '#type' => 'checkbox',
        '#title' => t('Set as active style'),
        '#default_value' => FALSE,
      );
      $form['load_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Load style'),
      );

      $form['delete_thid'] = array(
        '#type' => 'select',
        '#title' => t('Style'),
        '#options' => $existing_styles_options,
      );
      $form['delete_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete style'),
      );
    }

    return $form;
  }

  /**
   * Frontend form render.
   */
  function sweaver_form_render(&$vars, &$form, $plugin) {

    $name = $plugin['name'];
    $vars['tabs'][$name]['#tab_name'] = $form[$name]['#tab_name'];
    $vars['tabs_data'][$name]['#tab_description'] = $form[$name]['#tab_description'];

    // Containers.
    $output = '';
    foreach ($form[$name]['form']['#style_containers'] as $key => $container_value) {
      $output .= '<div class="container container-4"><div class="container-inner">';
      $output .= '<h2>'. $container_value['container_name'] .'</h2>';
      foreach ($container_value['properties'] as $key => $form_element) {
        if (isset($form[$name]['form'][$form_element])) {
          $output .= drupal_render($form[$name]['form'][$form_element]);
        }
      }
      $output .= '</div></div>';
    }

    $vars['tabs_data'][$name]['content'] = $output;
  }

  /**
   * Frontend form submit.
   */
  function sweaver_form_submit($form, &$form_state) {

    // Save style.
    if ($form_state['clicked_button']['#value'] == t('Save style')) {
      $update = array();
      $style = new stdClass;
      $theme_key = $form['#current_theme'];

      if ($form_state['values']['save_active'] == 1) {
        db_query("UPDATE {sweaver_style} set active = 0 where theme = '%s'", $theme_key);
      }

      if (!empty($form_state['values']['save_existing_style'])) {
        $update = array('thid');
        $style->thid = $form_state['values']['save_existing_style'];
        $style->style = $form['sweaver_plugin_styles']['form']['save_existing_style']['#options'][$style->thid];
      }
      else {
        $style->style = $form_state['values']['save_style_name'];
        if (empty($style->style)) {
          $this->sweaver_session(t('You need to enter a name for your style.'));
          return;
        }
      }

      $style->theme = $theme_key;
      $style->css = $form_state['values']['css'];
      $style->timestamp = time();
      $style->active = $form_state['values']['save_active'];
      drupal_write_record('sweaver_style', $style, $update);
      $form_state['saved_style_id'] = $style->thid;
      $form_state['saved_style_status'] = $style->active;
      $this->sweaver_session(t('The style @style has been saved.', array('@style' => $style->style)));

      // Write out CSS.
      $this->sweaver_export_file($form_state['values']['css_rendered'], $style);

      if ($style->active) {
        $this->sweaver_session(NULL, 'draft_mode', TRUE);
        $this->sweaver_session(NULL, 'loaded_style', TRUE);
      }
    }

    // Load style.
    if ($form_state['clicked_button']['#value'] == t('Load style')) {
      $this->sweaver_session(NULL, 'draft_mode', TRUE);
      $this->sweaver_session(NULL, 'loaded_style', TRUE);

      $thid = $form_state['values']['load_thid'];
      $active = $form_state['values']['load_active'];
      $theme_key = $form['#current_theme'];
      $style_name = $form['sweaver_plugin_styles']['form']['load_thid']['#options'][$thid];

      if (empty($thid)) {
        db_query("UPDATE {sweaver_style} set active = 0 where theme = '%s'", $theme_key);
      }
      else {
        if ($active == FALSE) {
          $this->sweaver_session(TRUE, 'draft_mode');
          $this->sweaver_session($thid, 'loaded_style');
          $this->sweaver_session(t('The style @style has been loaded and you are now working in draft mode.', array('@style' => $style_name)));
        }
        else {
          db_query("UPDATE {sweaver_style} set active = 0 WHERE theme = '%s'", $theme_key);
          db_query("UPDATE {sweaver_style} set active = 1 WHERE theme = '%s' AND thid = %d", $theme_key, $thid);
          $this->sweaver_session(t('The style @style has been loaded and set active.', array('@style' => $style_name)));
        }
      }
    }

    // Delete style.
    if ($form_state['clicked_button']['#value'] == t('Delete style')) {
      $this->sweaver_session(NULL, 'draft_mode', TRUE);
      $this->sweaver_session(NULL, 'loaded_style', TRUE);
      $thid = $form_state['values']['delete_thid'];
      db_query("DELETE FROM {sweaver_style} WHERE thid = %d", $thid);
      $style_name = $form['sweaver_plugin_styles']['form']['load_thid']['#options'][$thid];
      $this->sweaver_session(t('The style @style has been deleted.', array('@style' => $style_name)));
    }
  }

  /**
   * Export css to file.
   */
  function sweaver_export_file($css, $style) {
    $css = str_replace('<style type="text/css">', '', $css);
    $css = str_replace('</style>', '', $css);
    $filename = 'sweaver_' . $style->theme . '_'. $style->thid .'.css';

    // Create the css within the files folder.
    $path = file_create_path('sweaver');
    file_check_directory($path, FILE_CREATE_DIRECTORY);
    $file = $path .'/'. $filename;
    if (!$fp = fopen($file, 'w')) {
      $this->sweaver_session(t('The css file could not be created.'));
    }
    else {
      fwrite($fp, check_plain($css));
    }
    @fclose($fp);
  }

  /**
   * Menu callback
   */
  function menu_callback() {
    $form = array();

    // Settings.
    $form['sweaver_load_style_when_editor_inactive'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load style'),
      '#description' => t('Load active style for the theme when the editor is inactive. You can also add a line to the info file of your theme like this: stylesheets[all][] = /path/to/cssfile after you copied the file to your theme path. If you do that, you can ignore this setting completely and even disable this module after you have themed your website.'),
      '#default_value' => variable_get('sweaver_load_style_when_editor_inactive', TRUE),
    );

    $form = system_settings_form($form);

    // Styles list.
    $rows = array();
    $results = db_query("SELECT * FROM {sweaver_style} ORDER BY style ASC, theme ASC, active DESC")->fetch();
    foreach ($results as $style) {
      $row = array();
      $row[] = check_plain($style->style);
      $row[] = check_plain($style->theme);
      $row[] = ($style->active) ? t('Active') : t('Inactive');
      $operations = l(t('Delete'), 'admin/settings/sweaver/styles/delete/'. $style->thid);
      $row[] = $operations;
      $rows[] = $row;
    }

    if (empty($rows)) {
      $output = '<p>'. t('No styles found.') .'</p>';
    }
    else {

      $header = array(
        t('Style'),
        t('Theme'),
        t('Status'),
        t('Operations'),
      );

      // Styles list.
      $output = theme('table', $header, $rows);

    }

    $form['item'] = array(
      '#type' => 'item',
      '#value' => $output,
    );

    return $form;
  }

  /**
   * Menu callback, delete style.
   */
  function style_confirm_form_delete() {
    $thid = arg(5);
    $style = db_fetch_object(db_query('SELECT * FROM {sweaver_style} WHERE thid = %d', $thid));
    if ($style->thid) {
      $form['#style'] = $style;
      return confirm_form($form, t('Are you sure you want to delete style %style?', array('%style' => $style->style)), 'admin/settings/sweaver/styles');
    }
    else {
      drupal_set_message(t('Style not found'));
      drupal_goto('admin/config/user-interface/sweaver/styles');
    }
  }

  /**
   * Submit callback, delete style.
   */
  function style_confirm_form_delete_submit($form, &$form_state) {
    db_query("DELETE FROM {sweaver_style} WHERE thid = %d", $form['#style']->thid);
    drupal_set_message(t('Style %style has been removed', array('%style' => $form['#style']->style)));
    $form_state['redirect'] = 'admin/config/user-interface/sweaver/styles';
  }
}
