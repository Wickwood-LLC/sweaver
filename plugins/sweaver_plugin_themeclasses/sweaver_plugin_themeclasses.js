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
  var class_name = class_name.replace('spt-', '');
  $('.'+ class_name).css('border', '1px dotted blue');
}