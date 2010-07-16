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

Drupal.Sweaver.currentClass = '';
Drupal.Sweaver.ThemeClasses = function(class_name) {
  $('.'+ Drupal.Sweaver.currentClass).css('background-color', 'none');
  var class_name = class_name.replace('spt-', '');
  $('.'+ class_name).css('background-color', 'lime');
  Drupal.Sweaver.currentClass = class_name;
}