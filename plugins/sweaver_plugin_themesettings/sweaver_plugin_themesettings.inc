<?php
// $Id$

/**
 * @file
 * Theme settings plugin.
 */
class sweaver_plugin_themesettings extends sweaver_plugin {

  /**
   * Init function.
   */
  function sweaver_init() {
    // Override theme settings in $conf variable when in Draft.
    if ($this->sweaver_session(NULL, 'draft_mode')) {
      $theme_key = variable_get('theme_default', 'bartik');
      // Check with themeswitch.
      if ($theme_switch = $this->sweaver_session(NULL, 'sweaver_plugin_switch_theme')) {
        $theme_key = $theme_switch;
      }
      $css = $this->sweaver_get_current_style($theme_key);
      if (isset($css->themesettings) && !empty($css->themesettings)) {
        global $conf;
        $theme_settings_name = 'theme_' . $theme_key . '_settings';
        $conf[$theme_settings_name] = unserialize($css->themesettings);
      }
    }
  }

  /**
   * Frontend form.
   */
  function sweaver_form() {
    $form = array();
    $form['#popups'] = array();
    global $theme_key;

    // Get theme settings form.
    $themesettings = array();
    $css = $this->sweaver_get_current_style($theme_key);
    if (isset($css->themesettings) && !empty($css->themesettings)) {
      $themesettings = unserialize($css->themesettings);
    }
    $form = $this->sweaver_get_theme_settings_form($theme_key, $themesettings);

    // Add extra property for theme settings which need to be saved also.
    // Form alters on the theme settings form must be put those keys in this array.
    $form['#sweaver_other_themesettings'] = array();

    // Let other modules alter the form.
    $form['#method'] = 'post';
    $form_state['method'] = 'post';
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

        $form['item_' . $i] = array(
          '#markup' => '<div class="popup-link"><a href="#" id="theme-settings-link-' . $i . '">' . check_plain($title) . '</a></div>',
          '#weight' => $weight++,
        );

        if (isset($form[$key]['#prefix'])) {
          $form[$key]['#prefix'] .= '<div class="display-none" id="theme-settings-data-' . $i . '">';
        }
        else {
          $form[$key]['#prefix'] = '<div class="display-none" id="theme-settings-data-' . $i . '">';
        }

        if (isset($form[$key]['#suffix'])) {
          $form[$key]['#suffix'] = '</div>' . $form[$key]['#suffix'];
        }
        else {
          $form[$key]['#suffix'] = '</div>';
        }

        unset($form[$key]['#collapsible']);
        unset($form[$key]['#collapsed']);
        unset($form[$key]['#attributes']);

        // Add to popups.
        $form['#popups'][] = $key;

        $i++;
      }
    }

    // Deny access to the buttons for now, theme settings are
    // now also saved via the save style tab.
    $form['buttons']['#access'] = FALSE;

    return $form;
  }

  /**
   * Frontend form submit.
   */
  function sweaver_form_submit($form, &$form_state) {

    $values = $form_state['values'];
    $theme_key = $values['var'];
    $clicked_button = $form_state['clicked_button']['#value'];

    // Save style.
    if (($clicked_button == t('Save and continue') || $clicked_button == t('Save and publish') || $clicked_button == t('Publish style')) && isset($form_state['style_id'])) {

      $style_id = $form_state['style_id'];

      // General theme settings.
      $theme_values = array();
      $theme_settings_form = $form['sweaver_plugin_themesettings']['form']['theme_settings'];
      $theme_form_keys = element_children($theme_settings_form);

      // Logo and favicon.
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
        $this->sweaver_get_theme_specific_keys($form['sweaver_plugin_themesettings']['form']['theme_specific'], $theme_specific_keys, $fapi_types);
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

      // Rename logo / favicon when needed.
      foreach (array('logo_path', 'favicon_path') as $file) {

        $default = 'default_' . str_replace('_path', '', $file);

        // Rename when just uploaded.
        if (isset($form['sweaver_plugin_themesettings']['form']['#' . $file])) {
          $type = str_replace('_path', '', $file);
          $theme_name = $form['#current_theme'];
          $source_path = $form['sweaver_plugin_themesettings']['form']['#' . $file];
          $dest_draft = file_default_scheme() . '://sweaver/' . $type . '_' . $theme_name . '_' . $style_id . '_draft.' . $form['sweaver_plugin_themesettings']['form']['#' . $file . '_extension'];
          file_unmanaged_move($source_path, $dest_draft, FILE_EXISTS_REPLACE);
          $theme_values[$file] = $dest_draft;
          $theme_values[$default] = 0;
        }

        // Copy to live if save_live is found. Can either be directly after the upload
        // or when setting a style live after the uploads were done already.
        if ($form_state['publish'] && !$theme_values[$default] && !empty($theme_values[$file])) {
          $source_path = $theme_values[$file];
          $dest_live = str_replace('_draft', '_live', $theme_values[$file]);
          file_unmanaged_copy($source_path, $dest_live, FILE_EXISTS_REPLACE);
          $theme_values[$file] = $dest_live;
        }
      }

      // And finally save. Always save the draft version. If the
      // save_live key is found also update the live table.
      $theme_values_serialized = serialize($theme_values);
      db_query("UPDATE {sweaver_style_draft} set themesettings = :themesettings WHERE style_id = :style_id", array(':themesettings' => $theme_values_serialized, ':style_id' => $style_id));
      if ($form_state['publish']) {
        variable_set($theme_key, $theme_values);
        db_query("UPDATE {sweaver_style} set themesettings = :themesettings WHERE style_id = :style_id", array(':themesettings' => $theme_values_serialized, ':style_id' => $style_id));
      }

      // Fire other submit functions on the theme settings form.
      $submit_handlers = isset($form['sweaver_plugin_themesettings']['form']['#submit']) ? $form['sweaver_plugin_themesettings']['form']['#submit'] : array();
      foreach ($submit_handlers as $key => $function) {
        if (function_exists($function)) {
          $function($form['sweaver_plugin_themesettings']['form'], $form_state);
        }
      }
    }

    // Delete style.
    if (isset($form_state['style_to_delete'])) {
      $style = $form_state['style_to_delete'];

      // Reset theme settings when active.
      if ($form_state['style_active']) {
        variable_del($theme_key);
      }

      // Remove files.
      $dir = file_default_scheme() . '://sweaver';
      $mask = '/(.*)' . $style->theme . '_' . $style->style_id . '_(.*)/';
      $files = file_scan_directory($dir, $mask);
      foreach ($files as $key => $file) {
        file_unmanaged_delete($file->uri);
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
  function sweaver_get_theme_specific_keys($theme_specific_form, &$theme_specific_keys, $fapi_types) {
    $children = element_children($theme_specific_form);
    foreach ($children as $element) {
      if (in_array($theme_specific_form[$element]['#type'], $fapi_types)) {
        $theme_specific_keys[] = $element;
      }
      else {
        $this->sweaver_get_theme_specific_keys($theme_specific_form[$element], $theme_specific_keys, $fapi_types);
      }
    }
  }

  /**
   * Get theme settings form.
   * We basically copy the form from system.admin.inc. This makes it easer to
   * copy the uploads of the favicon & logo to revision versions of the style
   * and getting the right settings for the theme.
   *
   * People altering form and having also uploads wishing to work with sweaver must
   * handle the revisioning themeselves.
   *
   * @param $key
   *   The name of the theme.
   * @param $revision_theme_settings
   *   Revisions theme settings.
   */
  function sweaver_get_theme_settings_form($key, $revision_theme_settings) {
    $has_theme = FALSE;
    $form_state = array();

    $features = array();
    // Default settings are defined in theme_get_setting() in includes/theme.inc
    if ($key) {
      $var = 'theme_' . $key . '_settings';
      $themes = system_rebuild_theme_data();
      $features = $themes[$key]->info['features'];
    }
    else {
      $var = 'theme_settings';
    }

    $form['var'] = array('#type' => 'hidden', '#value' => $var);

    // Check for a new uploaded logo. Put it in the form
    // so we can give it the right filename in the submit function.
    if ($file = file_save_upload('logo_upload', array('file_validate_is_image' => array()))) {
      $parts = pathinfo($file->uri);
      $filename = file_default_scheme() . '://sweaver/temp_logo.' . $parts['extension'];
      $form['#logo_path'] = $filename;
      $form['#logo_path_extension'] = $parts['extension'];

      // The image was saved using file_save_upload() and was added to the
      // files table as a temporary file. We'll make a copy and let the garbage
      // collector delete the original upload.
      if (file_unmanaged_copy($file->uri, $filename, FILE_EXISTS_REPLACE)) {
        $_POST['default_logo'] = 0;
        $_POST['logo_path'] = $filename;
        $_POST['toggle_logo'] = 1;
      }
    }

    // Check for a new uploaded favicon. Put it in the form
    // so we can give it the right filename in the submit function.
    $validators = array('file_validate_extensions' => array('ico png gif jpg jpeg apng svg'));
    if ($file = file_save_upload('favicon_upload', $validators)) {
      $parts = pathinfo($file->uri);
      $filename = file_default_scheme() . '://sweaver/temp_favicon.' . $parts['extension'];
      $form['#favicon_path'] = $filename;
      $form['#favicon_path_extension'] = $parts['extension'];

      // The image was saved using file_save_upload() and was added to the
      // files table as a temporary file. We'll make a copy and let the garbage
      // collector delete the original upload.
      if (file_unmanaged_copy($file->uri, $filename)) {
        $_POST['default_favicon'] = 0;
        $_POST['favicon_path'] = $filename;
        $_POST['toggle_favicon'] = 1;
      }
    }

    // Toggle settings
    $toggles = array(
      'logo'                      => t('Logo'),
      'name'                      => t('Site name'),
      'slogan'                    => t('Site slogan'),
      'node_user_picture'         => t('User pictures in posts'),
      'comment_user_picture'      => t('User pictures in comments'),
      'comment_user_verification' => t('User verification status in comments'),
      'favicon'                   => t('Shortcut icon'),
      'main_menu'                 => t('Main menu'),
      'secondary_menu'            => t('Secondary menu'),
    );

    // Some features are not always available
    $disabled = array();
    if (!variable_get('user_pictures', 0)) {
      $disabled['toggle_node_user_picture'] = TRUE;
      $disabled['toggle_comment_user_picture'] = TRUE;
    }
    if (!module_exists('comment')) {
      $disabled['toggle_comment_user_picture'] = TRUE;
      $disabled['toggle_comment_user_verification'] = TRUE;
    }

    $form['theme_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Toggle display'),
      '#description' => t('Enable or disable the display of certain page elements.'),
    );
    foreach ($toggles as $name => $title) {
      if ((!$key) || in_array($name, $features)) {
        $form['theme_settings']['toggle_' . $name] = array('#type' => 'checkbox', '#title' => check_plain($title), '#default_value' => $this->sweaver_has_theme_setting('toggle_' . $name, $key, $revision_theme_settings));
        // Disable checkboxes for features not supported in the current configuration.
        if (isset($disabled['toggle_' . $name])) {
          $form['theme_settings']['toggle_' . $name]['#disabled'] = TRUE;
        }
      }
    }

    if (!element_children($form['theme_settings'])) {
      // If there is no element in the theme settings fieldset then do not show
      // it -- but keep it in the form if another module wants to alter.
      $form['theme_settings']['#access'] = FALSE;
    }

    // Logo settings
    if ((!$key) || in_array('logo', $features)) {
      $form['logo'] = array(
        '#type' => 'fieldset',
        '#title' => t('Logo image settings'),
        '#description' => t('If toggled on, the following logo will be displayed.'),
        '#attributes' => array('class' => array('theme-settings-bottom')),
      );
      $form['logo']['default_logo'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use the default logo'),
        '#default_value' => $this->sweaver_has_theme_setting('default_logo', $key, $revision_theme_settings),
        '#tree' => FALSE,
        '#description' => t('Check here if you want the theme to use the logo supplied with it.')
      );
      $form['logo']['logo_path'] = array(
        '#type' => 'textfield',
        '#title' => t('Path to custom logo'),
        '#default_value' => $this->sweaver_has_theme_setting('logo_path', $key, $revision_theme_settings),
        '#description' => t('The path to the file you would like to use as your logo file instead of the default logo.'));

      $form['logo']['logo_upload'] = array(
        '#type' => 'file',
        '#title' => t('Upload logo image'),
        '#maxlength' => 40,
        '#description' => t("If you don't have direct file access to the server, use this field to upload your logo.")
      );
    }

    if ((!$key) || in_array('favicon', $features)) {
      $form['favicon'] = array(
        '#type' => 'fieldset',
        '#title' => t('Shortcut icon settings'),
        '#description' => t("Your shortcut icon, or 'favicon', is displayed in the address bar and bookmarks of most browsers.")
      );
      $form['favicon']['default_favicon'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use the default shortcut icon.'),
        '#default_value' => $this->sweaver_has_theme_setting('default_favicon', $key, $revision_theme_settings),
        '#description' => t('Check here if you want the theme to use the default shortcut icon.')
      );
      $form['favicon']['favicon_path'] = array(
        '#type' => 'textfield',
        '#title' => t('Path to custom icon'),
        '#default_value' => $this->sweaver_has_theme_setting('favicon_path', $key, $revision_theme_settings),
        '#description' => t('The path to the image file you would like to use as your custom shortcut icon.')
      );

      $form['favicon']['favicon_upload'] = array(
        '#type' => 'file',
        '#title' => t('Upload icon image'),
        '#description' => t("If you don't have direct file access to the server, use this field to upload your shortcut icon.")
      );
    }

    if ($key) {
      // Call engine-specific settings.
      $function = $themes[$key]->prefix . '_engine_settings';
      if (function_exists($function)) {
        $form['engine_specific'] = array(
          '#type' => 'fieldset',
          '#title' => t('Theme-engine-specific settings'),
          '#description' => t('These settings only exist for the themes based on the %engine theme engine.', array('%engine' => $themes[$key]->prefix)),
        );
        $function($form, $form_state);
      }

      // Create a list which includes the current theme and all its base themes.
      if (isset($themes[$key]->base_themes)) {
        $theme_keys = array_keys($themes[$key]->base_themes);
        $theme_keys[] = $key;
      }
      else {
        $theme_keys = array($key);
      }

      // Save the name of the current theme (if any), so that we can temporarily
      // override the current theme and allow theme_get_setting() to work
      // without having to pass the theme name to it.
      $default_theme = !empty($GLOBALS['theme_key']) ? $GLOBALS['theme_key'] : NULL;
      $GLOBALS['theme_key'] = $key;

      // Process the theme and all its base themes.
      foreach ($theme_keys as $theme) {
        // Include the theme-settings.php file.
        $filename = DRUPAL_ROOT . '/' . str_replace("/$theme.info", '', $themes[$theme]->filename) . '/theme-settings.php';
        if (file_exists($filename)) {
          require_once $filename;
        }

        // Call theme-specific settings.
        $function = $theme . '_form_system_theme_settings_alter';
        if (function_exists($function)) {
          $function($form, $form_state);
        }
      }

      // Restore the original current theme.
      if (!is_null($default_theme)) {
        $GLOBALS['theme_key'] = $default_theme;
      }
      else {
        unset($GLOBALS['theme_key']);
      }
    }

    return $form;
  }

  /**
   * Helper function.
   * @param $revision_theme_settings
   * @param $theme_setting
   * @return unknown_type
   */
  function sweaver_has_theme_setting($theme_setting, $theme, $revision_theme_settings) {
    if (isset($revision_theme_settings[$theme_setting])) {
      return $revision_theme_settings[$theme_setting];
    }
    else {
      return theme_get_setting($theme_setting, $theme);
    }
  }
}
