/* $Id$ */

/**
 * See if the classes are found on the page.
 */
$(document).ready(function() {
  $('#sweaver_plugin_themeclasses .sweaver-text-float').each(function() {
    var class = $(this).attr('id').replace('spt-', '');
    if ($('.'+ class).length == 0) {
      $(this).hide();
    }
  });
});

/**
 * Switch to style editor and start editing the class.
 */

Drupal.Sweaver.ThemeClasses = function(class_name) {

  // Switch tabs.
  var remove_tab = 'tab-sweaver_plugin_themeclasses';
  var show_tab = 'tab-sweaver_plugin_editor';
  Drupal.Sweaver.switchTab(remove_tab, show_tab);

  // Update the values for the Style tab.
  var class_name = class_name.replace('spt-', '');
  var class_object = $('.'+ class_name);
  Drupal.Sweaver.updateStyleTab(class_name, class_object);
}