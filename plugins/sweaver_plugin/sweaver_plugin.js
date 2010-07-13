/* $Id$ */

Drupal.Sweaver = Drupal.Sweaver || {};


Drupal.Sweaver.writeCss = function(context) {

  var fullCss = '';

  $.each(Drupal.settings.sweaver['invokes'], function(index, module) {
    var invoke_function = module +'_updateCss';
    css = window[invoke_function].apply(this);
    if (css != '') {
      fullCss += css;
    }
  });

  // Store css in hidden field in save form
  $("#sweaver #edit-css").val($.toJSON(Drupal.Sweaver.css));

  // Add inline css
  $("#sweaver-form #edit-css").val(fullCss);
  fullCss = '<style type="text/css">' + fullCss + '</style>';
  $('#sweaver-css').html(fullCss);
  $('#edit-css-rendered').val(fullCss);
};

/**
 * Close/open style tab.
 */
Drupal.Sweaver.open = true;
Drupal.behaviors.styleClose = function(context) {
  $('#sweaver-tabs .close a').click(function(){
    if (Drupal.Sweaver.open == false) {
      $('#sweaver').css("height", "255px");
      Drupal.Sweaver.open = true;
    }
    else {
      $('#sweaver').css("height", "10px");
      Drupal.Sweaver.open = false;
    }
  });
}
$(document).ready(function() {
  Drupal.Sweaver.container = $('#sweaver-tabs .active-tab').attr('id').substr(4, $('#sweaver-tabs .active-tab').attr('id').length - 4);
  if ($('#edit-sweaver-editor-messages').val() != '') {
    alert($('#edit-sweaver-editor-messages').val());
  }
});

Drupal.behaviors.tabToggle = function(context) {
  $('#sweaver-tabs .tab a').click(function(){
    var container = $(this).parent().attr('id').replace('tab-', '');
    $(this).parent().siblings().removeClass('active-tab');
    $(this).parent().toggleClass('active-tab');
    $('#'+ container).show();
    $('#'+ Drupal.Sweaver.container).hide();
    Drupal.Sweaver.container = container;
  });
}
