/* $Id$ */

Drupal.Sweaver = Drupal.Sweaver || {};

Drupal.Sweaver.messageTimer = null;

Drupal.Sweaver.writeCss = function(context) {

  var fullCss = '';

  $.each(Drupal.settings.sweaver['invokes'], function(index, module) {
    var invoke_function = module +'_updateCss';
    css = window[invoke_function].apply(this);
    if (css != '') {
      fullCss += css;
    }
  });


  fullCss = '<style type="text/css">' + fullCss + '</style>';
  $('#sweaver-css').html(fullCss);
  $('#edit-css-rendered').val(fullCss);
};

/**
 * Close/open style tab.
 */
Drupal.Sweaver.open = true;

$(document).ready(function() {

  // Add area to print messages
  $('body').append('<div id="sweaver-messages"><div class="close">x</div><div class="message"></div></div>');  

  // Set bar height
  var contentHeight = 200;
  $('#sweaver-middle > div').each(function() {
    if ($(this).height() > contentHeight) contentHeight = $(this).height();
  });
  if (contentHeight > 350) contentHeight = 350;
  $('#sweaver-middle .sweaver-content').height(contentHeight);

  // open/close bar
  var activeTab = '';
  $('#sweaver-tabs .close a').click(function(){
    if (Drupal.Sweaver.open == false) {
      $('#sweaver').css("height", 'auto');
      $(this).parent().removeClass('active-tab');
      $('#' + activeTab).addClass('active-tab');
      Drupal.Sweaver.open = true;
    }
    else {
      $('#sweaver').css("height", 0);
      activeTab =  $('#sweaver-tabs .active-tab').attr('id');
      $('#sweaver-tabs .active-tab').removeClass('active-tab');
      $(this).parent().addClass('active-tab');
      Drupal.Sweaver.open = false;
    }
  });

  // toggle tabs
  Drupal.Sweaver.container = $('#sweaver-tabs .active-tab').attr('id').substr(4, $('#sweaver-tabs .active-tab').attr('id').length - 4);
  $('#sweaver-tabs .tab a').click(function(){
    var container = $(this).parent().attr('id').replace('tab-', '');
    if (container != Drupal.Sweaver.container) {
	    if (!Drupal.Sweaver.open) {
        $('#sweaver').css("height", 'auto');
        Drupal.Sweaver.open = true;
      }
		  $(this).parent().siblings().removeClass('active-tab');
		  $(this).parent().toggleClass('active-tab');
		  $('#'+ container).show();
		  $('#'+ Drupal.Sweaver.container).hide();
		  Drupal.Sweaver.container = container;
		} else {
      if (!Drupal.Sweaver.open) {
        $(this).parent().siblings().removeClass('active-tab');
        $(this).parent().toggleClass('active-tab');
        $('#sweaver').css("height", 'auto');
        Drupal.Sweaver.open = true;
      }
		}
  });

  // Print messages if any
  if ($('#edit-sweaver-editor-messages').val() != '') {
    Drupal.Sweaver.setMessage($('#edit-sweaver-editor-messages').val());
  }
  
  // Close messages
  $('#sweaver-messages .close').click(function(){
    $('#sweaver-messages').hide();
    clearTimeout(Drupal.Sweaver.messageTimer);   
  });
});


/**
 * Display Sweaver messages.
 */
Drupal.Sweaver.setMessage = function(message) {
  messageLeft = 7;
  messageTop = $(window).height() - $('#sweaver').outerHeight() - $('#sweaver-messages').outerHeight() - $('#sweaver-tabs').outerHeight() - 7;
  $('#sweaver-messages .message').html(message)
  $('#sweaver-messages').css({'left' : messageLeft, 'top' : messageTop}).fadeIn('fast');
	Drupal.Sweaver.messageTimer = window.setTimeout(function() {$('#sweaver-messages').fadeOut('normal');}, 5000);  
}