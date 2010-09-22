/* $Id$ */

(function ($) {

/**
 * Implementation of HOOK_updateCss().
 *
 * Return custom css.
 */
sweaver_plugin_customcss_updateCss = function() {
  var fullCss = '';
  fullCss = $('#edit-sweaver-plugin-custom-css').val();
  return fullCss;
}

/**
 * Preview button onclick behavior.
 */
Drupal.behaviors.SweaverCustomCss = {
  attach: function (context) {
    $('#edit-sweaver-plugin-custom-css-button').click(function(){
      Drupal.Sweaver.writeCss();
      Drupal.Sweaver.setMessage(Drupal.t('Your custom css has been succesfully applied.'), 5000);
      return false;
    });
  }
}

})(jQuery);