<?php
// $Id$

/**
 * @file
 * Base class for a sweaver plugin.
 */
class sweaver_plugin {

  /**
   * Sweaver dependencies.
   */
  function sweaver_dependencies() {
    return array();
  }

  /**
   * Sweaver objects alter.
   */
  function sweaver_objects_alter(&$objects) {}

  /**
   * Menu registry.
   */
  function sweaver_menu(&$weight, $page_arguments, $base) {
    $items = array();

    // Enable or disable plugins.
    $page_arguments['callback_method'] = 'sweaver_plugin_config_plugins';
    $items['admin/config/user-interface/sweaver/plugins'] = $base + array(
      'title' => 'Plugins',
      'page arguments' => array($page_arguments),
      'type' => MENU_LOCAL_TASK,
      'weight' => 2,
    );

    return $items;
  }

  /**
   * Theme registry.
   */
  function sweaver_theme() {
    $theme_functions = array();

    $skin = variable_get('sweaver_skin', SWEAVER_SKIN);
    $plugin_path = drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin';
    $skin_path = drupal_get_path('module', 'sweaver') .'/skins/'. $skin;
    $theme_functions = array(
      'sweaver_plugin' => array(
        'template' => $skin,
        'file' => $skin . '.theme.inc',
        'path' => $skin_path,
        'render element' => 'form',
      ),
      'sweaver_plugin_config_plugins' => array(
        'template' => 'sweaver-plugin-config-plugins',
        'file' => 'sweaver_plugin.theme.inc',
        'path' => $plugin_path,
        'render element' => 'form',
      ),
    );

    return $theme_functions;
  }

  /**
   * Init function.
   */
  function sweaver_init() {}

  /**
   * Sweaver form.
   */
  function sweaver_form() {}

  /**
   * Sweaver submit handler.
   */
  function sweaver_form_submit($form, &$form_state) {}

  /**
   * Sweaver form render.
   */
  function sweaver_form_render(&$vars, &$form, $plugin) {
    $name = $plugin['name'];
    if (isset($form[$name])) {

      if (isset($form[$name]['form']['#popups'])) {
        $this->sweaver_popups_render($vars, $form, $name);
      }

      if (isset($form[$name]['#tab_name'])) {
        $vars['tabs'][$name]['#tab_name'] = $form[$name]['#tab_name'];
      }
      $vars['tabs_data'][$name]['#tab_description'] = $form[$name]['#tab_description'];
      $vars['tabs_data'][$name]['content'] = drupal_render($form[$name]['form']);
    }
  }

  /**
   * Helper function render the popups.
   */
  function sweaver_popups_render(&$vars, &$form, $name) {
    foreach ($form[$name]['form']['#popups'] as $key => $form_key) {
      $vars['sweaver_popup'] .= drupal_render($form[$name]['form'][$form_key]);
    }
  }

  /**
   * Sweaver css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    $skin = variable_get('sweaver_skin', SWEAVER_SKIN);
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.js');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/skins/'. $skin .'/'. $skin .'.css');
  }

  /**
   * Menu callback.
   */
  function sweaver_menu_callback() {}

  /**
   * Menu callback validate.
   */
  function sweaver_menu_callback_validate($form, &$form_state) {}

  /**
   * Menu callback submit.
   */
  function sweaver_menu_callback_submit($form, &$form_state) {}

  /**
   * Images handler.
   */
  function sweaver_images_handler(&$images) {}

  /**
   * Session handler for sweaver.
   *
   * @param $value
   *   The value hold by the session key.
   * @param $session_key
   *   Name of the session key.
   * @param $action
   *   Action to perform. TRUE will remove the session key.
   * @param $return_string
   *   Whether to return the value of the session key as string
   *   or simply return the value.
   * @return
   *   Either FALSE, a value from the key or nothing.
   */
  function sweaver_session($value = NULL, $session_key = 'sweaver_editor_messages', $remove = FALSE, $return_string = TRUE) {
    // Add value to session key.
    if (isset($session_key) && isset($value) && !$remove) {
      if (!isset($_SESSION[$session_key])) $_SESSION[$session_key] = array();
      $_SESSION[$session_key][] = $value;
    }
    // Remove complete session key.
    elseif (isset($_SESSION[$session_key]) && !isset($value) && $remove) {
      if (isset($_SESSION[$session_key])) unset($_SESSION[$session_key]);
    }
    // Return values from session key.
    elseif (isset($_SESSION[$session_key])) {
      if ($return_string) {
        $return = FALSE;
        $values = $_SESSION[$session_key];
        if (count($values) > 1) {
          foreach ($values as $key => $value) {
            $return .= $value ."\n";
          }
        }
        elseif (isset($values[0])) {
          $return = $values[0];
        }
        return $return;
      }
      else {
        return $_SESSION[$session_key];
      }
    }
  }

  /**
   * Return a style for a theme.
   *
   * @param $theme
   *   The machine name of the theme.
   * @param $reset
   *   Whether to reset the current $css variable or not.
   * @return $css
   *   The css definition for this theme.
   */
  function sweaver_get_current_style($theme = NULL, $reset = FALSE) {
    $run = &drupal_static('run', FALSE);
    $css = &drupal_static('css', FALSE);

    if (!$run || $reset) {

      $run = TRUE;
      if ($this->sweaver_session(NULL, 'sweaver_temp')) {
        ctools_include('object-cache');
        $css = ctools_object_cache_get('sweaver-styling', 'sweaver-styling');
        $css->type = 'draft';
      }
      elseif ($this->sweaver_session(NULL, 'draft_mode')) {
        $table = ($this->sweaver_session(NULL, 'loaded_table') == 'live') ? 'sweaver_style' : 'sweaver_style_draft';
        $css = db_query("SELECT * FROM {". $table ."} where style_id = :style_id", array(':style_id' => $this->sweaver_session(NULL, 'loaded_style')))->fetchObject();
        $css->type = 'draft';
      }
      else {
        $css = db_query("SELECT style_id, theme, style, css, customcss, themesettings, active FROM {sweaver_style} where theme = :theme and active = 1", array(':theme' => $theme))->fetchObject();
        $css->type = 'live';
      }
      if (!isset($css->style_id)) {
        $css = NULL;
      }
    }
    return $css;
  }

  /**
   * Get theme info.
   *
   * @param $theme
   *   The machine name for the theme.
   * @return $theme_info
   *   The info about a theme.
   */
  function sweaver_get_theme_info($theme) {
    $info = db_query("SELECT info FROM {system} WHERE name = :name AND type = 'theme'", array(':name' => $theme))->fetchField();
    $theme_info = $info;
    return $theme_info;
  }

  /**
   * Return all enabled themes.
   */
  function sweaver_get_all_themes() {
    $all_themes = &drupal_static('all_themes', array());
    if (empty($themes)) {
      $all_themes = array();
      $themes = db_query("SELECT filename, name, status, info FROM {system} WHERE type = 'theme' AND status = '1' ORDER BY name ASC")->fetchAll();
      foreach ($themes as $theme) {
        if ($theme->status) {
          $theme->info = unserialize($theme->info);
          $theme->prefix = $theme->info['engine'];
          $all_themes[$theme->name] = $theme;
        }
      }
    }
    return $all_themes;
  }

  /**
   * Clear cache and page requisites.
   */
  function sweaver_clear_cache() {
    cache_clear_all();
    drupal_clear_css_cache();
  }

  /**
   * Plugins form.
   */
  function sweaver_plugin_config_plugins() {
    $weight = 100;
    $form = array();
    $form['#tree'] = TRUE;
    $form['#plugins'] = array();
    $form['#theme'] = 'sweaver_plugin_config_plugins';
    $plugins_order = variable_get('sweaver_plugins_weight', array());
    cache_clear_all('plugins:sweaver:plugins', 'cache');
    cache_clear_all('sweaver_plugins', 'cache');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.admin.css');

    foreach (sweaver_get_plugin() as $plugin) {
      $plugin_name = $plugin['name'];
      $object = sweaver_get_plugin($plugin_name);

      // Do not add sweaver plugin.
      if ($plugin_name == 'sweaver_plugin') {
        continue;
      }

      $default_weight = isset($plugins_order[$plugin_name]) ? $plugins_order[$plugin_name] : $weight++;
      $form['#plugins'][$plugin_name] = $default_weight;

      // Status - editor can not be disabled.
      $status = variable_get('sweaver_plugin_status_'. $plugin_name, FALSE);
      if ($plugin_name == 'sweaver_plugin_editor') {
       $form[$plugin_name]['status'] = array(
          '#type' => 'checkbox',
          '#value' => 1,
          '#access' => FALSE,
        );
      }
      else {
        $missing = array();
        $can_be_enabled = TRUE;
        $dependencies = $object->sweaver_dependencies();
        if (!empty($dependencies)) {
          foreach ($dependencies as $module) {
            if (!module_exists($module)) {
              $can_be_enabled = FALSE;
              $missing[] = $module;
            }
          }
        }
        $form[$plugin_name]['status'] = array(
          '#type' => 'checkbox',
          '#default_value' => $status,
        );
        if (!$can_be_enabled) {
          $form[$plugin_name]['status']['#disabled'] = TRUE;
          $form[$plugin_name]['status']['#value'] = 0;
          $form[$plugin_name]['status']['#description'] = t('Following modules or plugins are disabled or missing: @module', array('@module' => implode(', ', $missing)));
        }
      }

      // Markup.
      $form[$plugin_name]['name'] = array(
        '#markup' => isset($plugin['handler']['tab']) ? $plugin['handler']['tab'] : drupal_ucfirst($plugin_name),
      );
      $form[$plugin_name]['weight'] = array(
        '#type' => 'weight',
        '#delta' => 50,
        '#attributes' => array(
          'class' => array('plugin-weight'),
        ),
        '#default_value' => $default_weight,
      );
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );

    return $form;
  }

  /**
   * Plugins form submit callback.
   */
  function sweaver_plugin_config_plugins_submit(&$form, &$form_state) {
    // Reset cookies.
    setcookie('sweaver_active_tab', 'sweaver_plugin_editor');

    $plugins_order = array();
    foreach ($form['#plugins'] as $plugin_name => $weight) {
      variable_set('sweaver_plugin_status_'. $plugin_name, $form_state['values'][$plugin_name]['status']);
      $plugins_order[$plugin_name] = $form_state['values'][$plugin_name]['weight'];
    }
    variable_set('sweaver_plugins_weight', $plugins_order);
    drupal_flush_all_caches();

    drupal_set_message(t('The configuration options have been saved.'));
  }

  /**
   * Sweaver editor containers.
   */
  function sweaver_containers() {
    return array(
      'one' => t('Font'),
      'two' => t('Background'),
      'three' => t('Spacing'),
      'four' => t('Empty'),
    );
  }
}
