<?php
// $Id$

/**
 * @file
 * Base class for a sweaver plugin.
 */
class sweaver_plugin {

  /**
   * Sweaver enabled.
   */
  function sweaver_enabled($plugin_name) {
    return variable_get('sweaver_plugin_status_'. $plugin_name, TRUE);
  }

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
    $items['admin/settings/sweaver/plugins'] = $base + array(
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

    $plugin_path = drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin';
    $theme_functions = array(
      'sweaver_plugin' => array(
        'template' => 'sweaver-plugin',
        'file' => 'sweaver_plugin.theme.inc',
        'path' => $plugin_path,
        'arguments' => array('form' => NULL),
      ),
      'sweaver_plugin_config_plugins' => array(
        'template' => 'sweaver-plugin-config-plugins',
        'file' => 'sweaver_plugin.theme.inc',
        'path' => $plugin_path,
        'arguments' => array('form' => NULL),
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

      // Popups.
      if (isset($form[$name]['form']['#popups'])) {
        foreach ($form[$name]['form']['#popups'] as $key => $form_key) {
          $vars['sweaver_popup'] .= drupal_render($form[$name]['form'][$form_key]);
        }
      }

      if (isset($form[$name]['#tab_name'])) {
        $vars['tabs'][$name]['#tab_name'] = $form[$name]['#tab_name'];
      }
      $vars['tabs_data'][$name]['#tab_description'] = $form[$name]['#tab_description'];
      $vars['tabs_data'][$name]['content'] = drupal_render($form[$name]['form']);
    }
  }

  /**
   * Sweaver css and js.
   */
  function sweaver_form_css_js(&$inline_settings) {
    jquery_ui_add('ui.draggable');
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.js', 'module');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.css', 'module');
  }

  /**
   * Menu callback.
   */
  function menu_callback() {}

  /**
   * Menu callback validate.
   */
  function menu_callback_validate($form, &$form_state) {}


  /**
   * Menu callback submit.
   */
  function menu_callback_submit($form, &$form_state) {}

  /**
   * Images handler.
   */
  function sweaver_images_handler(&$images) {
  }

  /**
   * Session handler for sweaver.
   *
   * @param mixed $value
   *   The value hold by the session key.
   * @param string $session_key
   *   Name of the session key.
   * @param boolean $action
   *   Action to perform. TRUE will remove the session key.
   * @param boolean $return_string
   *   Whether to return the value of the session key as string
   *   or simply return the value.
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
   * @param string $theme
   *   The machine name of the theme.
   * @param boolean $reset
   *   Whether to reset the current $css variable or not.
   * @return stdClass $css
   *   The css definition for this theme.
   */
  function sweaver_get_current_style($theme = NULL, $reset = FALSE) {
    static $css = NULL;
    static $run = FALSE;

    if (!$run || $reset) {
      $sweaver = sweaver_get_plugin('sweaver_plugin');

      $run = TRUE;
      if ($sweaver->sweaver_session(NULL, 'draft_mode')) {
        $css = db_fetch_object(db_query("SELECT * FROM {sweaver_style} where thid = %d AND theme = '%s'", $sweaver->sweaver_session(NULL, 'loaded_style'), $theme));
      }
      else {
        $css = db_fetch_object(db_query("SELECT * FROM {sweaver_style} where theme = '%s' and active = 1", $theme));
      }
      if (isset($css->thid)) {
        return $css;
      }
    }
    return $css;
  }

  /**
   * Get theme info.
   *
   * @param string $theme
   *   The machine name for the theme.
   * @return array $theme_info
   *   The info about a theme.
   */

  function sweaver_get_theme_info($theme) {
    $info = db_result(db_query("SELECT info FROM {system} WHERE name = '%s' AND type ='theme'", $theme));
    $theme_info = unserialize($info);
    return $theme_info;
  }

   /**
   * Plugins form.
   */
  function sweaver_plugin_config_plugins() {
    $form = array();
    $weight = 100;
    $form['#tree'] = TRUE;
    $form['#plugins'] = array();
    $form['#theme'] = 'sweaver_plugin_config_plugins';
    $plugins_order = variable_get('sweaver_plugins_weight', array());
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin/sweaver_plugin.css');

    foreach (sweaver_get_plugin() as $plugin) {
      $plugin_name = $plugin['name'];
      $object = sweaver_get_plugin($plugin_name);

      // Do not add sweaver plugin.
      if ($plugin_name == 'sweaver_plugin') {
        continue;
      }

      $default_weight = in_array($plugin_name, $plugins_order) ? array_search($plugin_name, $plugins_order) : $weight++;
      $form['#plugins'][$default_weight] = $plugin_name;

      // Status - editor can not be disabled.
      $status = $object->sweaver_enabled($plugin_name);
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
          $form[$plugin_name]['status']['#description'] = t('Following modules are missing: @module', array('@module' => implode(', ', $missing)));
        }
      }

      // Markup.
      $form[$plugin_name]['name'] = array(
        '#type' => 'markup',
        '#value' => isset($plugin['handler']['tab']) ? $plugin['handler']['tab'] : drupal_ucfirst($plugin_name),
      );
      $form[$plugin_name]['weight'] = array(
        '#type' => 'weight',
        '#delta' => 50,
        '#attributes' => array(
          'class' => 'plugin-weight',
        ),
        '#default_value' => $default_weight,
      );
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );

    return $form;
  }

  /**
   * Plugins form submit callback.
   */
  function sweaver_plugin_config_plugins_submit(&$form, &$form_state) {
    drupal_set_message(t('Your settings have been saved.'));

    // Remove all cookies.
    unset($_COOKIE['sweaver_open']);
    unset($_COOKIE['sweaver_active_tab']);
    unset($_COOKIE['sweaver_height']);

    $plugins_order = array();
    foreach ($form['#plugins'] as $key => $plugin_name) {
      variable_set('sweaver_plugin_status_'. $plugin_name, $form_state['values'][$plugin_name]['status']);
      $plugins_order[$form_state['values'][$plugin_name]['weight']] = $plugin_name;
    }
    variable_set('sweaver_plugins_weight', $plugins_order);
    drupal_flush_all_caches();
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
