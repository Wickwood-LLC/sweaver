/* $Id$ */

/**
 * See if the classes are found on the page.
 */
$(document).ready(function() {
  $('#sweaver .sweaver-plugin-themeclasses').each(function() {
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

  // TODO refactor this and in sweaver_plugin_editor.js to make this a separate function.

  // Switch tabs.
  var remove = 'tab-sweaver_plugin_themeclasses';
  var show = 'tab-sweaver_plugin_editor';
  var container_show = 'sweaver_plugin_editor';
  var container_remove = 'sweaver_plugin_themeclasses';

  $('#'+ remove).removeClass('active-tab');
  $('#'+ show).toggleClass('active-tab');
  $('#'+ container_show + ' > div').show();
  $('#'+ container_remove + ' > div').hide();
  Drupal.Sweaver.container = container_show;

  Drupal.Sweaver.activeTab = show;
  Drupal.Sweaver.cookie('sweaver_active_tab', show);

  // Fill the activeElement and update the Form and ActivePath.
  var class_name = class_name.replace('spt-', '');
  var class_object = $('.'+ class_name);

  Drupal.Sweaver.activeElement.id = '';
  Drupal.Sweaver.activeElement.class = new Array(class_name);
  Drupal.Sweaver.activeElement.tag = 'div';
  Drupal.Sweaver.activeElement.type = 'block';
  Drupal.Sweaver.activeElement.object = class_object;

  $('#sweaver_plugin_editor .sweaver-header').html('<div id="selected-path" class="clear-block"></div>');
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'class' : Drupal.Sweaver.activeElement.class, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'object' : Drupal.Sweaver.activeElement.object});
  Drupal.Sweaver.addToFullPath(class_object, 0, true);
  Drupal.Sweaver.printActivePath(0, class_object);
  Drupal.Sweaver.updateForm();
}