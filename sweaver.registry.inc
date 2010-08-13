<?php
// $Id$

/**
 * @file
 * Registry for Sweaver.
 */

/**
 * Menu items.
 */
function _sweaver_menu() {
  $weight = 0;
  $items = array();

  $base = array(
    'access arguments' => array('configure sweaver'),
    'page callback' => 'sweaver_menu_callback',
  );

  foreach (sweaver_get_plugin() as $plugin) {
    $plugin_name = $plugin['name'];
    $object = sweaver_get_plugin($plugin_name);
    if ($object->sweaver_enabled($plugin_name)) {
      $page_arguments = array(
        'plugin' => $plugin_name,
      );
      $items += $object->sweaver_menu($weight, $page_arguments, $base);
    }
  }

  return $items;
}

/**
 * Theming functions.
 */
function _sweaver_theme() {
  $theme_functions = array();

  foreach (sweaver_get_plugin() as $plugin) {
    $plugin_name = $plugin['name'];
    $object = sweaver_get_plugin($plugin_name);
    if ($object->sweaver_enabled($plugin_name)) {
      $theme_functions += $object->sweaver_theme();
    }
  }
  return $theme_functions;
}

/**
 * Sweaver plugins.
 */
function _sweaver_sweaver_plugins() {
  $plugins = array();

  $plugins['sweaver_plugin'] = array(
    'handler' => array(
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin',
      'file' => 'sweaver_plugin.inc',
      'class' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_editor'] = array(
    'handler' => array(
      'tab' => t('Style'),
      'tab_description' => t('Click on the element you want to theme.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_editor',
      'file' => 'sweaver_plugin_editor.inc',
      'class' => 'sweaver_plugin_editor',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_styles'] = array(
    'handler' => array(
      'tab' => t('Load/Save'),
      'tab_description' => t('Save or load the current style.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_styles',
      'file' => 'sweaver_plugin_styles.inc',
      'class' => 'sweaver_plugin_styles',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_customcss'] = array(
    'handler' => array(
      'tab' => t('Custom css'),
      'tab_description' => t('Add custom css.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_customcss',
      'file' => 'sweaver_plugin_customcss.inc',
      'class' => 'sweaver_plugin_customcss',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_images'] = array(
    'handler' => array(
      'tab' => t('Images'),
      'tab_description' => t('Upload images to use as a background image.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_images',
      'file' => 'sweaver_plugin_images.inc',
      'class' => 'sweaver_plugin_images',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_themeswitch'] = array(
    'handler' => array(
      'tab' => t('Switch theme'),
      'tab_description' => t('Switch and style another theme.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeswitch',
      'file' => 'sweaver_plugin_themeswitch.inc',
      'class' => 'sweaver_plugin_themeswitch',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_themesettings'] = array(
    'handler' => array(
      'tab' => t('Theme settings'),
      'tab_description' => t('Configure the settings for this theme.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themesettings',
      'file' => 'sweaver_plugin_themesettings.inc',
      'class' => 'sweaver_plugin_themesettings',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_themeclasses'] = array(
    'handler' => array(
      'tab' => t('Theme styles'),
      'tab_description' => t('Select an item - if any - to style it. Clicking will switch to the style editor form.'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_themeclasses',
      'file' => 'sweaver_plugin_themeclasses.inc',
      'class' => 'sweaver_plugin_themeclasses',
      'parent' => 'sweaver_plugin',
    ),
  );
  $plugins['sweaver_plugin_fontface'] = array(
    'handler' => array(
      'tab' => t('Font face'),
      'path' => drupal_get_path('module', 'sweaver') .'/plugins/sweaver_plugin_fontface',
      'file' => 'sweaver_plugin_fontface.inc',
      'class' => 'sweaver_plugin_fontface',
      'parent' => 'sweaver_plugin',
    ),
  );
  return $plugins;
}

/**
 * Default selectors.
 */
function _sweaver_default_sweaver_selector() {
  $selectors = array();

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'body';
  $selector->description = 'Body';
  $selector->selector_selector = 'body';
  $selector->selector_highlight = FALSE;
  $selectors['body'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'link';
  $selector->description = 'a link';
  $selector->selector_selector = 'a';
  $selector->selector_highlight = FALSE;
  $selectors['link'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'div';
  $selector->description = 'a region';
  $selector->selector_selector = 'div';
  $selector->selector_highlight = FALSE;
  $selectors['div'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'span';
  $selector->description = 'an item';
  $selector->selector_selector = 'span';
  $selector->selector_highlight = FALSE;
  $selectors['span'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'ul';
  $selector->description = 'an unordered list';
  $selector->selector_selector = 'ul';
  $selector->selector_highlight = FALSE;
  $selectors['ul'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'ol';
  $selector->description = 'an ordered list';
  $selector->selector_selector = 'ol';
  $selector->selector_highlight = FALSE;
  $selectors['ol'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'li';
  $selector->description = 'a list item';
  $selector->selector_selector = 'li';
  $selector->selector_highlight = FALSE;
  $selectors['li'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'h1';
  $selector->description = 'Heading 1';
  $selector->selector_selector = 'h1';
  $selector->selector_highlight = FALSE;
  $selectors['h1'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'h2';
  $selector->description = 'Heading 2';
  $selector->selector_selector = 'h2';
  $selector->selector_highlight = FALSE;
  $selectors['h2'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'h3';
  $selector->description = 'Heading 3';
  $selector->selector_selector = 'h3';
  $selector->selector_highlight = FALSE;
  $selectors['h3'] = $selector;

  $selector = new stdClass;
  $selector->api_version = 1;
  $selector->disabled = FALSE;
  $selector->name = 'p';
  $selector->description = 'a paragraph';
  $selector->selector_selector = 'p';
  $selector->selector_highlight = FALSE;
  $selectors['p'] = $selector;

  return $selectors;
}

/**
 * Default properties.
 */
function _sweaver_default_sweaver_property() {
  $propertys = array();

  // Font family.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'font-family';
  $property->description = 'Font family';
  $property->property_parent = '';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        '' => 'Theme default',
        'Arial, Helvetica, Sans-Serif' => 'Arial',
        '\'Arial Black\', Sans-Serif' => 'Arial Black',
        '\'Comic Sans MS\', Cursive' => 'Comic Sans MS',
        '\'Courier New\', Courier, Monospace' => 'Courier',
        'Georgia, Serif' => 'Georgia',
        'Helvetica, Arial, Sans-Serif' => 'Helvetica',
        'Impact, Charcoal, Sans-Serif' => 'Impact',
        '\'Lucida Sans\', \'Lucida Grande\', Sans-Serif' => 'Lucida',
        '\'Palatino Linotype\', Palatino, Serif' => 'Palatino',
        'Tahoma, Geneva, Sans-Serif' => 'Tahoma',
        '\'Times New Roman\', Times, Serif' => 'Times New Roman',
        '\'Trebuchet\', Helvetica, Sans-Serif' => 'Trebuchet',
        'Verdana, Geneva, Sans-Serif' => 'Verdana',
      );
  $propertys['font-family'] = $property;

  // Font style.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'font-style';
  $property->description = 'Font style';
  $property->property_parent = '';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'normal' => 'Normal',
        'italic' => 'Italic',
      );
  $propertys['font-style'] = $property;

  // Font weight.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'font-weight';
  $property->description = 'Font weight';
  $property->property_parent = '';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'normal' => 'Normal',
        'bold' => 'Bold',
      );
  $propertys['font-weight'] = $property;

  // Font size group
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'font-size-wrapper';
  $property->description = 'Font size';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['font-size-wrapper'] = $property;

  // Font size
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'font-size';
  $property->description = 'size';
  $property->property_parent = 'font-size-wrapper';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 1;
  $property->property_slider_max = 200;
  $property->property_options = array();
  $propertys['font-size'] = $property;

  // Line height
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'line-height';
  $property->description = 'line height';
  $property->property_parent = 'font-size-wrapper';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 1;
  $property->property_slider_max = 200;
  $property->property_options = array();
  $propertys['line-height'] = $property;

  // Text alignment.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'text-align';
  $property->description = 'Text alignment';
  $property->property_parent = '';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'left' => 'Left',
        'right' => 'Right',
        'justify' => 'Justified',
      );
  $propertys['text-align'] = $property;

  // Color.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'color';
  $property->description = 'Font color';
  $property->property_parent = '';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['color'] = $property;

  // Background.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'background-color';
  $property->description = 'Background color';
  $property->property_parent = '';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['background-color'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'background-image';
  $property->description = 'Background image';
  $property->property_parent = '';
  $property->property_type = 'image';
  $property->property_prefix = 'url(';
  $property->property_suffix = ')';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['background-image'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'background-repeat';
  $property->description = 'Background repeat';
  $property->property_parent = '';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'no-repeat' => 'None',
        'repeat-x' => 'Horizontally',
        'repeat-y' => 'Vertically',
        'repeat' => 'Both',
      );
  $propertys['background-repeat'] = $property;

  // Padding.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'padding';
  $property->description = 'Padding';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['padding'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'padding-top';
  $property->description = 'Top';
  $property->property_parent = 'padding';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['padding-top'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'padding-left';
  $property->description = 'Left';
  $property->property_parent = 'padding';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['padding-left'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'padding-right';
  $property->description = 'Right';
  $property->property_parent = 'padding';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['padding-right'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'padding-bottom';
  $property->description = 'Bottom';
  $property->property_parent = 'padding';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['padding-bottom'] = $property;

  // Margin.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'margin';
  $property->description = 'Margin';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['margin'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'margin-top';
  $property->description = 'Top';
  $property->property_parent = 'margin';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['margin-top'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'margin-left';
  $property->description = 'Left';
  $property->property_parent = 'margin';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['margin-left'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'margin-right';
  $property->description = 'Right';
  $property->property_parent = 'margin';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['margin-right'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'margin-bottom';
  $property->description = 'Bottom';
  $property->property_parent = 'margin';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['margin-bottom'] = $property;

  // Top border.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-top';
  $property->description = 'Border top';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-top'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-top-width';
  $property->description = 'Width';
  $property->property_parent = 'border-top';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-top-width'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-top-color';
  $property->description = 'Color';
  $property->property_parent = 'border-top';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-top-color'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-top-style';
  $property->description = 'Style';
  $property->property_parent = 'border-top';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'none' => 'None',
        'dashed' => 'Dashed',
        'dotted' => 'Dotted',
        'solid' => 'Solid',
        'double' => 'Double',
        'groove' => 'Groove',
        'inset' => 'Inset',
        'outset' => 'Outset',
        'ridge' => 'Ridge',
  );
  $propertys['border-top-style'] = $property;

  // Left border.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-left';
  $property->description = 'Border left';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-left'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-left-width';
  $property->description = 'Width';
  $property->property_parent = 'border-left';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_options = array();
  $propertys['border-left-width'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-left-color';
  $property->description = 'Color';
  $property->property_parent = 'border-left';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-left-color'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-left-style';
  $property->description = 'Style';
  $property->property_parent = 'border-left';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'none' => 'None',
        'dashed' => 'Dashed',
        'dotted' => 'Dotted',
        'solid' => 'Solid',
        'double' => 'Double',
        'groove' => 'Groove',
        'inset' => 'Inset',
        'outset' => 'Outset',
        'ridge' => 'Ridge',
  );
  $propertys['border-left-style'] = $property;

  // Right border.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-right';
  $property->description = 'Border right';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-right'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-right-width';
  $property->description = 'Width';
  $property->property_parent = 'border-right';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_options = array();
  $propertys['border-right-width'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-right-color';
  $property->description = 'Color';
  $property->property_parent = 'border-right';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-right-color'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-right-style';
  $property->description = 'Style';
  $property->property_parent = 'border-right';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'none' => 'None',
        'dashed' => 'Dashed',
        'dotted' => 'Dotted',
        'solid' => 'Solid',
        'double' => 'Double',
        'groove' => 'Groove',
        'inset' => 'Inset',
        'outset' => 'Outset',
        'ridge' => 'Ridge',
  );
  $propertys['border-right-style'] = $property;

  // Bottom border.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-bottom';
  $property->description = 'Border bottom';
  $property->property_parent = '';
  $property->property_type = 'parent';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-bottom'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-bottom-width';
  $property->description = 'Width';
  $property->property_parent = 'border-bottom';
  $property->property_type = 'slider';
  $property->property_prefix = 'border-bottom-width';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 500;
  $property->property_options = array();
  $propertys['border-bottom-width'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-bottom-color';
  $property->description = 'Color';
  $property->property_parent = 'border-bottom';
  $property->property_type = 'color';
  $property->property_prefix = '#';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array();
  $propertys['border-bottom-color'] = $property;

  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'border-bottom-style';
  $property->description = 'Style';
  $property->property_parent = 'border-bottom';
  $property->property_type = 'select';
  $property->property_prefix = '';
  $property->property_suffix = '';
  $property->property_slider_min = '';
  $property->property_slider_max = '';
  $property->property_options = array(
        'none' => 'None',
        'dashed' => 'Dashed',
        'dotted' => 'Dotted',
        'solid' => 'Solid',
        'double' => 'Double',
        'groove' => 'Groove',
        'inset' => 'Inset',
        'outset' => 'Outset',
        'ridge' => 'Ridge',
  );
  $propertys['border-bottom-style'] = $property;

  // Width.
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'width';
  $property->description = 'Width';
  $property->property_parent = '';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 2000;
  $property->property_options = array();
  $propertys['width'] = $property;

  // Height
  $property = new stdClass;
  $property->api_version = 1;
  $property->disabled = FALSE;
  $property->name = 'height';
  $property->description = 'Height';
  $property->property_parent = '';
  $property->property_type = 'slider';
  $property->property_prefix = '';
  $property->property_suffix = 'px';
  $property->property_slider_min = 0;
  $property->property_slider_max = 2000;
  $property->property_options = array();
  $propertys['height'] = $property;

  return $propertys;
}

/**
 * sweaver types.
 */
function _sweaver_default_sweaver_type() {
  $types = array();

  $type = new stdClass;
  $type->api_version = 1;
  $type->disabled = FALSE;
  $type->name = 'block';
  $type->description = 'Block';
  $type->type_options = array(
    'font-family' => 'font-family',
    'font-style' => 'font-style',
    'font-weight' => 'font-weight',
    'font-size' => 'font-size',
    'line-height' => 'line-height',
    'text-align' => 'text-align',
    'color' => 'color',
    'background-color' => 'background-color',
    'background-image' => 'background-image',
    'background-repeat' => 'background-repeat',
    'padding-top' => 'padding-top',
    'padding-left' => 'padding-left',
    'padding-right' => 'padding-right',
    'padding-bottom' => 'padding-bottom',
    'margin-top' => 'margin-top',
    'margin-left' => 'margin-left',
    'margin-right' => 'margin-right',
    'margin-bottom' => 'margin-bottom',
    'border-top-style' => 'border-top-style',
    'border-top-color' => 'border-top-color',
    'border-top-width' => 'border-top-width',
    'border-left-style' => 'border-left-style',
    'border-left-color' => 'border-left-color',
    'border-left-width' => 'border-left-width',
    'border-right-style' => 'border-right-style',
    'border-right-color' => 'border-right-color',
    'border-right-width' => 'border-right-width',
    'border-bottom-style' => 'border-bottom-style',
    'border-bottom-color' => 'border-bottom-color',
    'border-bottom-width' => 'border-bottom-width',
    'width' => 'width',
    'height' => 'height',
  );
  $types['block'] = $type;

  $type = new stdClass;
  $type->api_version = 1;
  $type->disabled = FALSE;
  $type->name = 'inline';
  $type->description = 'Inline';
  $type->type_options = array(
    'font-family' => 'font-family',
    'font-style' => 'font-style',
    'font-weight' => 'font-weight',
    'font-size' => 'font-size',
    'line-height' => 'line-height',
    'color' => 'color',
    'background-color' => 'background-color',
    'background-image' => 'background-image',
    'background-repeat' => 'background-repeat',
    'border-top-style' => 'border-top-style',
    'border-top-color' => 'border-top-color',
    'border-top-width' => 'border-top-width',
    'border-left-style' => 'border-left-style',
    'border-left-color' => 'border-left-color',
    'border-left-width' => 'border-left-width',
    'border-right-style' => 'border-right-style',
    'border-right-color' => 'border-right-color',
    'border-right-width' => 'border-right-width',
    'border-bottom-style' => 'border-bottom-style',
    'border-bottom-color' => 'border-bottom-color',
    'border-bottom-width' => 'border-bottom-width',
  );
  $types['inline'] = $type;

  $type = new stdClass;
  $type->api_version = 1;
  $type->disabled = FALSE;
  $type->name = 'list-item';
  $type->description = 'List item';
  $type->type_options = array(
    'font-family' => 'font-family',
    'font-style' => 'font-style',
    'font-weight' => 'font-weight',
    'font-size' => 'font-size',
    'line-height' => 'line-height',
    'text-align' => 'text-align',
    'color' => 'color',
    'background-color' => 'background-color',
    'background-image' => 'background-image',
    'background-repeat' => 'background-repeat',
    'padding-top' => 'padding-top',
    'padding-left' => 'padding-left',
    'padding-right' => 'padding-right',
    'padding-bottom' => 'padding-bottom',
    'margin-top' => 'margin-top',
    'margin-left' => 'margin-left',
    'margin-right' => 'margin-right',
    'margin-bottom' => 'margin-bottom',
    'border-top-style' => 'border-top-style',
    'border-top-color' => 'border-top-color',
    'border-top-width' => 'border-top-width',
    'border-left-style' => 'border-left-style',
    'border-left-color' => 'border-left-color',
    'border-left-width' => 'border-left-width',
    'border-right-style' => 'border-right-style',
    'border-right-color' => 'border-right-color',
    'border-right-width' => 'border-right-width',
    'border-bottom-style' => 'border-bottom-style',
    'border-bottom-color' => 'border-bottom-color',
    'border-bottom-width' => 'border-bottom-width',
    'width' => 'width',
    'height' => 'height',
  );
  $types['list-item'] = $type;

  return $types;
}
