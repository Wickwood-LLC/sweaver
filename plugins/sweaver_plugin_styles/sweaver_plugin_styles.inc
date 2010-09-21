<?php
// $Id$

/**
 * @file
 * Styles plugin.
 */

define('SWEAVER_STYLE_SAVE', 0);
define('SWEAVER_STYLE_PUBLISH', 1);

class sweaver_plugin_styles extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function sweaver_menu(&$weight, $page_arguments, $base) {

    $items = array();

    $page_arguments = array(
      'plugin' => 'sweaver_plugin_styles',
    );

    // Styles administration.
    $items['admin/config/user-interface/sweaver/styles'] = $base + array(
      'title' => 'Styles',
      'page arguments' => array($page_arguments),
      'type' => MENU_LOCAL_TASK,
      'weight' => $weight++,
    );

    // Delete a style.
    $page_arguments['callback_method'] = 'sweaver_style_confirm_form_delete';
    $items['admin/config/user-interface/sweaver/styles/delete'] = $base + array(
      'title' => 'Delete style',
      'page arguments' => array($page_arguments),
      'type' => MENU_CALLBACK,
    );

    // Autosave callback.
    $base['access arguments'] = array('use editor');
    $page_arguments['callback_method'] = 'sweaver_autosave';
    $items['sweaver-autosave'] = $base + array(
      'title' => 'Autosave',
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
    $form['#popups'] = array();
    global $theme_key;

    // Current style.
    $current_style = $this->sweaver_get_current_style($theme_key);

    // Saved styles in database.
    $existing_styles = FALSE;
    $existing_styles_options = array();

    // Draft versions.
    $drafts = db_query("SELECT style_id, style FROM {sweaver_style_draft} where theme = :theme", array(':theme' => $theme_key))->fetchAll();
    foreach ($drafts as $draft) {
      $existing_styles_options[$draft->style_id] = $draft->style;
    }
    if (count($existing_styles_options) > 0) {
      $existing_styles = TRUE;
    }
    // Live versions.
    $lives = db_query("SELECT style_id, style FROM {sweaver_style} where theme = :theme AND active = 1", array(':theme' => $theme_key))->fetchAll();
    foreach ($lives as $live) {
      $existing_styles_options[$live->style_id] = $live->style;
    }

    // Save popup.
    $form['save_style_popup']['save_style'] = array(
      '#type' => 'textfield',
      '#size' => 50,
      '#weight' => 1,
    );
    if ($existing_styles) {

      // Hide new style name by default.
      $form['save_style_popup']['save_style']['#attributes'] = array('class' => 'display-none');

      $form['save_style_popup']['save_type'] = array(
        '#type' => 'select',
        '#options' => array(
          0 => t('New style'),
          1 => t('Existing style'),
        ),
        '#default_value' => 1,
        '#attributes' => array('class' => array('radio-style-save-type')),
        '#weight' => 0,
      );

      $form['save_style_popup']['style_existing_id'] = array(
        '#type' => 'select',
        '#options' => $existing_styles_options,
        '#default_value' => (isset($current_style->style_id)) ? $current_style->style_id : 0,
        '#weight' => 2,
      );
    }
    else {
      // If no existing styles, set style save type to new.
      $form['save_style_popup']['save_type'] = array(
        '#type' => 'value',
        '#value' => 0,
      );
      $form['save_style_popup']['style_existing_id'] = array(
        '#type' => 'value',
        '#value' => 0,
      );
    }
    $form['save_style_popup']['save_keep_draft'] = array(
      '#type' => 'checkbox',
      '#title' => t('Keep working on this style after saving.'),
      '#weight' => 3,
    );
    $form['save_style_popup']['save_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save style'),
      '#weight' => 4,
    );

    // Load popup.
    if ($existing_styles) {

      $load_style_options = $existing_styles_options;
      // Add the active versions of a style.
      $active_results = db_query("SELECT style_id, style FROM {sweaver_style} WHERE active = 1");
      while ($active = db_fetch_object($active_results)) {
        $load_style_options[$active->style_id .'_live_table'] = $active->style .' ('. t('live') .')';
      }
      // Fresh start.
      $load_style_options[0] = t('Fresh style');

      $form['load_style_popup']['load_style'] = array(
        '#type' => 'select',
        '#options' => $load_style_options,
      );
      $form['load_style_popup']['load_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Load style'),
      );
    }

    // Publish popup. Make sure we don't save the temporary one.
    if (isset($current_style->style_id) && !empty($current_style->style_id)) {
      $form['publish_style_popup']['publish_style'] = array(
        '#type' => 'markup',
        '#value' => t('Set style %stylename visible for your visitors. Any unsaved changes will also be saved when publishing.', array('%stylename' => $current_style->style))
      );
      $form['publish_style_popup']['publish_id'] = array(
        '#type' => 'hidden',
        '#value' => $current_style->style_id,
      );
      $form['publish_style_popup']['publish_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Publish style'),
      );
    }

    // Delete popup.
    if ($existing_styles && variable_get('sweaver_styles_delete_tab', FALSE)) {

      // Question.
      $form['delete_style_popup']['question']['delete_style'] = array(
        '#type' => 'select',
        '#options' => $existing_styles_options,
      );
      $form['delete_style_popup']['question']['delete_confirm'] = array(
        '#type' => 'button',
        '#value' => t('Delete'),
      );
      $form['delete_style_popup']['question']['#prefix'] = '<div class="delete-style-confirm">';
      $form['delete_style_popup']['question']['#suffix'] = '</div>';

      // Confirmation.
      $form['delete_style_popup']['warning'] = array(
        '#type' => 'item',
        '#value' => '<div>'. t('Are you sure you want to delete the style ? All changes and files will be lost. If the style is also active, your visitors will see the default theming.') .'</div>',
      );
      $form['delete_style_popup']['warning']['delete_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete style'),
      );
      $form['delete_style_popup']['warning']['delete_cancel'] = array(
        '#type' => 'button',
        '#value' => t('Cancel'),
      );
      $form['delete_style_popup']['warning']['#prefix'] = '<div class="delete-style-question display-none clearfix">';
      $form['delete_style_popup']['warning']['#suffix'] = '</div>';
    }

    // Build the popup links & content.
    $i = 0;
    $form['#popups_links'] = '';
    $form['#popups_styles'] = array(
      'save_style_popup' => array(
        'title' => t('Save'),
        'description' => 'Save a style for yourself. It will only be visible for your visitors after publishing.',
      ),
      'load_style_popup' => array(
        'title' => t('Load'),
        'description' => t('Load a style to continue working on it. It will only be visible for your visitors after publishing.'),
      ),
      'publish_style_popup' => array(
        'title' => t('Publish'),
        'description' => '',
      ),
      'delete_style_popup' => array(
        'title' => t('Delete'),
        'description' => t('Delete a style which you do not want to use anymore.')
      ),
    );
    foreach ($form['#popups_styles'] as $key => $action) {
      if (isset($form[$key])) {
        ++$i;
        $form['#popups'][] = $key;
        $form['#popups_links'] .= '<div class="style-actions-link"><a href="#" id="style-actions-link-'. $i .'">'. $action['title'] .'</a></div>';
        $form[$key]['#prefix'] = '<div class="display-none '. str_replace('_', '-', $key) .'" id="style-actions-data-'. $i .'"><h2>'. t('@action style', array('@action' => $action['title'])) .'</h2><p>'. $action['description'] .'</p>';
        $form[$key]['#suffix'] = '</div>';
      }
    }

    return $form;
  }

  /**
   * Frontend form render.
   */
  function sweaver_form_render(&$vars, &$form, $plugin) {
    $vars['style_actions'] = $form['sweaver_plugin_styles']['form']['#popups_links'];
    $this->sweaver_popups_render($vars, $form, $plugin['name']);
  }

  /**
   * Frontend css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    if (variable_get('sweaver_styles_autosave', 10) > 0) {
      $inline_settings['sweaver']['autosave'] = variable_get('sweaver_styles_autosave', 10);
    }
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_styles/sweaver_plugin_styles.js', 'module');
  }

  /**
   * Frontend form submit.
   */
  function sweaver_form_submit($form, &$form_state) {
    $path = file_create_path('sweaver');
    file_check_directory($path, FILE_CREATE_DIRECTORY);

    $styles_form = $form['sweaver_plugin_styles']['form'];
    $clicked_button = $form_state['clicked_button']['#value'];

    // Save style.
    if ($clicked_button == t('Save style') || $clicked_button == t('Publish style')) {
      // Reset session.
      $this->sweaver_reset_style_session();

      $update = array();
      $style = new stdClass;
      $theme_key = $form['#current_theme'];
      $submit_type = ($clicked_button == t('Save style')) ? SWEAVER_STYLE_SAVE : SWEAVER_STYLE_PUBLISH;

      // Create variables from form_state, easier to re-use them in the flow.
      $save_type = ($submit_type == SWEAVER_STYLE_SAVE) ? $form_state['values']['save_type'] : TRUE;
      $save_keep_draft = ($submit_type == SWEAVER_STYLE_SAVE) ? $form_state['values']['save_keep_draft'] : FALSE;
      $style_name = ($submit_type == SWEAVER_STYLE_SAVE) ? $form_state['values']['save_style'] : '';
      $style_existing_id = ($submit_type == SWEAVER_STYLE_SAVE) ? $form_state['values']['style_existing_id'] : $form_state['values']['publish_id'];

      if ($save_type) {
        $update = array('style_id');
        $style->style_id = $style_existing_id;
        $style->style = $styles_form['save_style_popup']['style_existing_id']['#options'][$style->style_id];
      }
      else {
        $style->style = $style_name;
        if (empty($style->style)) {
          $this->sweaver_session(t('You need to enter a name for your style.'));
          return;
        }
      }

      // Save the draft version.
      $style->theme = $theme_key;
      $style->css = $form_state['values']['css'];
      drupal_write_record('sweaver_style_draft', $style, $update);
      $this->sweaver_export_file($form_state['values']['css_rendered'], $style);
      $form_state['style_id'] = $style->style_id;
      $form_state['publish'] = $submit_type;

      // Publish style.
      if ($submit_type == SWEAVER_STYLE_PUBLISH) {
        $style->active = 1;
        $this->sweaver_export_file($form_state['values']['css_rendered'], $style, 'live');
        $message = 'The style @style has been published.';
        db_query("UPDATE {sweaver_style} set active = 0 WHERE theme = '%s'", $style->theme);
        // Find out first if this style already exists or not.
        if (db_result(db_query("SELECT style_id FROM {sweaver_style} WHERE style_id = %d", $style->style_id)) === FALSE) {
          $update = array();
        }
        drupal_write_record('sweaver_style', $style, $update);
      }
      else {
        // Keep working in draft mode.
        if ($save_keep_draft) {
          $this->sweaver_session(TRUE, 'draft_mode');
          $this->sweaver_session($style->style_id, 'loaded_style');
          $this->sweaver_session('draft', 'loaded_table');
          $message = 'The style @style has been saved. You can keep working on your style.';
        }
        // Leave draft mode.
        else {
          $message = 'The style @style has been saved. To work on the style again, you need to load it.';
        }
      }

      $this->sweaver_session(t($message, array('@style' => $style->style)));
    }

    // Load style.
    if ($clicked_button == t('Load style')) {
      // Reset session.
      $this->sweaver_reset_style_session();

      // Build message and session variables.
      $theme_key = $form['#current_theme'];
      $style_id = $form_state['values']['load_style'];
      $style_name = $styles_form['load_style_popup']['load_style']['#options'][$style_id];

      // Load from draft or live table ?
      $table = 'draft';
      $pos = strpos($style_id, '_live_table');
      if ($pos !== FALSE) {
        $table = 'live';
        $style_id = str_replace('_live_table', '', $style_id);
      }

      $this->sweaver_session(TRUE, 'draft_mode');
      $this->sweaver_session($style_id, 'loaded_style');
      $this->sweaver_session($table, 'loaded_table');
      $this->sweaver_session(t('The style @style has been loaded. It is only visible for you.', array('@style' => $style_name)));
    }

    // Delete style.
    if ($clicked_button == t('Delete style')) {

      // Reset session.
      $this->sweaver_reset_style_session();

      // Get info from db and store in form_state so other modules can profit from it.
      $style_id = $form_state['values']['delete_style'];
      $style = db_fetch_object(db_query("SELECT * FROM {sweaver_style_draft} WHERE style_id = %d", $style_id));
      $style_live = db_fetch_object(db_query("SELECT * FROM {sweaver_style} WHERE style_id = %d", $style_id));
      $form_state['style_active'] = isset($style_live->active) ? $style_live->active : FALSE;
      $form_state['style_to_delete'] = $style;

      // Delete entries from tables, files.
      $this->sweaver_delete_style($style);

      // Message.
      $style_name = $styles_form['delete_style_popup']['question']['delete_style']['#options'][$style_id];
      $this->sweaver_session(t('The style @style has been deleted.', array('@style' => $style_name)));
    }

    // Clear page & block cache and page requisites.
    $this->sweaver_clear_cache();
  }

  /**
   * Export css to file.
   */
  function sweaver_export_file($css, $style, $type = 'draft') {
    $css = str_replace('<style type="text/css">', '', $css);
    $css = str_replace('</style>', '', $css);
    $filename = 'sweaver_' . $style->theme . '_'. $style->style_id .'_'. $type .'.css';

    // Create the css within the files folder.
    $path = file_create_path('sweaver');
    file_check_directory($path, FILE_CREATE_DIRECTORY);
    $file = $path .'/'. $filename;
    if (!$fp = fopen($file, 'w')) {
      $this->sweaver_session(t('The css file could not be created.'));
    }
    else {
      // Clean the css before writing it.
      $css = $this->sweaver_ctools_css_filter($css, FALSE);
      fwrite($fp, $css);
    }
    @fclose($fp);
  }

  /**
   * Filter a chunk of CSS text. Based on CTools, but we want the
   * regex for url out. This might become a setting too later.
   */
  function sweaver_ctools_css_filter($css, $compressed = TRUE) {
    ctools_include('css');
    $css_data = ctools_css_disassemble($css);

    $empty_array = array();
    $disallowed_values_regex = '/(expression)/';
    $filtered = ctools_css_filter_css_data($css_data, $empty_array, $empty_array, '', $disallowed_values_regex);

    return $compressed ? ctools_css_compress($filtered) : ctools_css_assemble($filtered);
  }

  /**
   * Delete a complete style.
   */
  function sweaver_delete_style($style) {
    db_query("DELETE FROM {sweaver_style} WHERE style_id = %d", $style->style_id);
    db_query("DELETE FROM {sweaver_style_draft} WHERE style_id = %d", $style->style_id);
    $draft = file_directory_path() .'/sweaver/sweaver_'. $style->theme .'_'. $style->style_id .'_draft.css';
    $live = file_directory_path() .'/sweaver/sweaver_'. $style->theme .'_'. $style->style_id .'_live.css';
    file_delete($draft);
    file_delete($live);

    // Remove files tied to theme.
    $dir = file_directory_path() .'/sweaver';
    $mask = '(.*)'. $style->theme .'_'. $style->style_id .'_(.*)';
    $files = file_scan_directory($dir, $mask);
    foreach ($files as $key => $file) {
      file_delete($file->filename);
    }
  }

  /**
   * Reset style session.
   */
  function sweaver_reset_style_session() {
    $this->sweaver_session(NULL, 'loaded_table', TRUE);
    $this->sweaver_session(NULL, 'draft_mode', TRUE);
    $this->sweaver_session(NULL, 'loaded_style', TRUE);
    $this->sweaver_session(NULL, 'sweaver_temp', TRUE);
    ctools_include('object-cache');
    ctools_object_cache_clear('sweaver-styling', 'sweaver-styling');
  }

  /**
   * Menu callback
   */
  function sweaver_menu_callback() {
    $form = array();

    // Settings.
    $form['sweaver_load_style_when_editor_inactive'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load style'),
      '#description' => t('Load active style for the theme when the editor is inactive. You can also add a line to the info file of your theme like this: stylesheets[all][] = /path/to/cssfile after you copied the file to your theme path. If you do that, you can ignore this setting completely and even disable this module after you have themed your website.'),
      '#default_value' => variable_get('sweaver_load_style_when_editor_inactive', TRUE),
    );
    $form['sweaver_styles_delete_tab'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show delete tab'),
      '#description' => t('Show the delete tab in the frontend editor.'),
      '#default_value' => variable_get('sweaver_styles_delete_tab', FALSE),
    );
    $form['sweaver_styles_autosave'] = array(
      '#title' => t('Autosave'),
      '#type' => 'select',
      '#options' => array(
        0 => t('Never'),
        5 => t('Every 5 seconds'),
        10 => t('Every 10 seconds'),
        15 => t('Every 15 seconds'),
        30 => t('Every 30 seconds'),
        45 => t('Every 45 seconds'),
        60 => t('Every minute'),
        120 => t('Every two minutes'),
      ),
      '#default_value' => variable_get('sweaver_styles_autosave', 10),
      '#description' => t('Check for changes on your style and custom CSS every x seconds. If a change has been identified, sweaver will save those settings in a temporary cache table with AJAX. So leaving a page - or even worse, a browser crash - will make sure you keep your current configuration.'),
    );
    $form = system_settings_form($form);

    // Styles list.
    $rows = array();
    $styles = db_query("SELECT ssd.*, ss.active FROM {sweaver_style_draft} ssd LEFT JOIN {sweaver_style} ss on ss.style_id = ssd.style_id ORDER BY ssd.style ASC, ssd.theme ASC, ss.active DESC")->fetch();
    foreach ($styles as $style) {
      $row = array();
      $row[] = check_plain($style->style);
      $row[] = str_replace('_', ' ', check_plain($style->theme));
      $row[] = ($style->active) ? t('active') : t('inactive');
      $operations = l(t('Delete'), 'admin/settings/sweaver/styles/delete/'. $style->style_id);
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
  function sweaver_style_confirm_form_delete() {
    $style_id = arg(5);
    $style = db_fetch_object(db_query('SELECT * FROM {sweaver_style_draft} WHERE style_id = %d', $style_id));
    if ($style->style_id) {
      $form['#style'] = $style;
      return confirm_form($form, t('Are you sure you want to delete style %style?', array('%style' => $style->style)), 'admin/settings/sweaver/styles');
    }
    else {
      drupal_set_message(t('Style not found.'));
      drupal_goto('admin/settings/sweaver/styles');
    }
  }

  /**
   * Submit callback, delete style.
   */
  function sweaver_style_confirm_form_delete_submit($form, &$form_state) {
    $style = $form['#style'];
    $this->sweaver_delete_style($style);
    $this->sweaver_clear_cache();
    drupal_set_message(t('Style %style has been removed', array('%style' => $form['#style']->style)));
    $form_state['redirect'] = 'admin/settings/sweaver/styles';
  }

  /**
   * Autosave post.
   */
  function sweaver_autosave() {
    // Prevent caching of JS output.
    $GLOBALS['conf']['cache'] = FALSE;
    // Prevent Devel from hi-jacking our output in any case.
    $GLOBALS['devel_shutdown'] = FALSE;

    // Save current styling.
    $style = new stdClass;
    $style->style_id = 0;
    $style->style = t('Temporary');
    $style->css = $_POST['css'];
    $style->customcss = $_POST['customcss'];
    $style->themesettings = '';

    // Get the themesettings if applicable and overwrite style id & style name.
    if ($this->sweaver_session(NULL, 'draft_mode')) {
      $table = ($this->sweaver_session(NULL, 'loaded_table') == 'live') ? 'sweaver_style' : 'sweaver_style_draft';
      $original = db_fetch_object(db_query("SELECT style_id, style, theme, themesettings FROM {". $table ."} where style_id = %d", $this->sweaver_session(NULL, 'loaded_style')));
      if (isset($original->style_id)) {
        $style->style_id = $original->style_id;
        $style->style = $original->style;
        $style->theme = $original->theme;
        $style->themesettings = $style->themesettings;
      }
    }

    // Save to CTools object cache.
    ctools_include('object-cache');
    ctools_object_cache_set('sweaver-styling', 'sweaver-styling', $style);

    // Set session variable.
    $this->sweaver_session(TRUE, 'sweaver_temp');

    // Exit.
    exit(drupal_json(array('error' => 0)));
  }
}

