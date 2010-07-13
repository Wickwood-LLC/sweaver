/* $Id$ */

/**
 * Implementation of Drupal.HOOK_updateCss().
 * 
 * Return custom css.
 */
sweaver_plugin_customcss_updateCss = function() {
  var fullCss = '';
  fullCss = $('.sweaver_plugin_custom_css').val();
  return fullCss;
}

/**
 * Preview button onclick behavior.
 */
Drupal.behaviors.SweaverCustomCss = function(context) {
  $('.sweaver_plugin_custom_css_button').click(function(){
    Drupal.Sweaver.writeCss();
    Drupal.Sweaver.setMessage(Drupal.t('Your custom css has been succesfully applied.'));
    return false;
  });
}