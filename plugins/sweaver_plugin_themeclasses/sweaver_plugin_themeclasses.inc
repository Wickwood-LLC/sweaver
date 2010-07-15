<?php
// $Id$

/**
 * Base class for a sweaver plugin.
 */

class sweaver_plugin_themeclasses extends sweaver_plugin {

  function is_enabled($plugin_name) {
    return variable_get('sweaver_plugin_status_'. $plugin_name, TRUE);
  }

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {

    $items = array();
    $base = array(
      'load arguments' => array('%map'),
      'access arguments' => array('configure sweaver'),
      'file' => 'plugins/sweaver_plugin_themeclasses/sweaver_plugin_themeclasses.admin.inc',
    );

    // Theme classes list.
    $items['admin/settings/sweaver/themeclasses'] = $base + array(
      'title' => 'Theme classes',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_themeclasses_list'),
      'type' => MENU_LOCAL_TASK,
      'weight' => $weight,
    );

    return $items;
  }

  /**
   * Frontend form.
   */
  function frontend_form() {
    $form = array();

    $class_groups = trim(variable_get('sweaver_themeclasses_list', 'skinr'));
    if (empty($class_groups)) {
      return $form;
    }

    global $theme_key;
    $theme_info = sweaver_get_theme_info($theme_key);
    if (empty($theme_info)) {
      return $form;
    }


    $cache = variable_get('sweaver_plugin_themeclasses_'. $theme_key, array());
    if (!empty($cache)) {
      $styles = $cache['styles'];
    }
    else {
      $styles = array();
      $groups = explode("\n", $class_groups);
      foreach ($groups as $group) {
        $group = trim($group);
        if (isset($theme_info[$group])) {
          $this->get_styles($theme_info[$group], $styles);
        }
      }

      // Cache the stuff.
      variable_set('sweaver_plugin_themeclasses_'. $theme_key, array('checked' => TRUE, 'styles' => $styles));
    }

    if (!empty($styles)) {
      $content = t('<h2>Styles</h2>!styles', array('!styles' => implode(' ', $styles)));
    }
    else {
      $content = t('<h2>No styles found</h2>');
    }

    $form['sweaver_plugin_themeclasses'] = array(
      '#type' => 'markup',
      '#value' => $content,
    );

    return $form;
  }


  /**
   * Frontend css and js.
   */
  function frontend_css_js(&$inline_settings) {
    drupal_add_js(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeclasses/sweaver_plugin_themeclasses.js', 'module');
    drupal_add_css(drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeclasses/sweaver_plugin_themeclasses.css', 'theme');
  }

  /**
   * Get styles from the group. We look for a key called 'class'.
   */
  function get_styles($theme_info, &$styles) {
    foreach ($theme_info as $key => $value) {
      if (is_array($value) && isset($value['class'])) {
        $label = isset($value['label']) ? $value['label'] : $value['class'];
        $styles[] = '<div class="sweaver-plugin-themeclasses" id="spt-'. $value['class'] .'"><a href="javascript:Drupal.Sweaver.ThemeClasses(\'spt-'. $value['class'] .'\')">'. strip_tags($value['label']) .'</a></div>';
      }
      if (is_array($value)) {
        $this->get_styles($theme_info[$key], $styles);
      }
    }
  }
}
