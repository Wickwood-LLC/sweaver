/* $Id$ */

(function ($) {

Drupal.Sweaver = Drupal.Sweaver || {};

Drupal.Sweaver.messageTimer = null;

Drupal.Sweaver.writeCss = function(context) {

  var fullCss = '';
  //var window = $(self);

  $.each(Drupal.settings.sweaver['invokes'], function(index, module) {
    var invoke_function = module +'_updateCss()';
    //console.log(invoke_function);
    //css = window[invoke_function].apply(this);
    // @todo using eval() for now, need to find
    // out alternative in this jQuery version.
    css = eval(invoke_function);
    if (css != '') {
      fullCss += css;
    }
  });

  fullCss = '<style type="text/css">' + fullCss + '</style>';
  $('#sweaver-css').html(fullCss);
  $('#edit-css-rendered').val(fullCss);
};

$(document).ready(function() {

  Drupal.Sweaver.activeTab = (Drupal.Sweaver.cookie('sweaver_active_tab') == null) ? $('#sweaver-tabs .tab:first').attr('id')  : Drupal.Sweaver.cookie('sweaver_active_tab');
  Drupal.Sweaver.open = (Drupal.Sweaver.cookie('sweaver_open') == null) ? 'true' : Drupal.Sweaver.cookie('sweaver_open');

  // Set bar height
	if (Drupal.Sweaver.cookie('sweaver_height') == null) {
	  var contentHeight = 200;
	  $('#sweaver-middle .sweaver-content').each(function() {
	    if ($(this).outerHeight() > contentHeight) contentHeight = $(this).outerHeight();
	  });
	  if (contentHeight > 350) contentHeight = 350;
	  $('#sweaver-middle .sweaver-content').height(contentHeight);
	  Drupal.Sweaver.cookie('sweaver_height', contentHeight);
  }

  // open/close bar
  $('#sweaver-tabs .close a').click(function(){
    if (Drupal.Sweaver.open == 'false') {
      $('#sweaver').css('height', 'auto');
      $(this).parent().removeClass('active-tab');
      $('#' + Drupal.Sweaver.activeTab).addClass('active-tab');
      Drupal.Sweaver.open = 'true';
    }
    else {
      $('#sweaver').css("height", 0);
      Drupal.Sweaver.activeTab =  $('#sweaver-tabs .active-tab').attr('id');
      $(this).parent().addClass('active-tab');
      Drupal.Sweaver.open = 'false';
      Drupal.Sweaver.cookie('sweaver_active_tab', Drupal.Sweaver.activeTab);
    }
    Drupal.Sweaver.cookie('sweaver_open', Drupal.Sweaver.open);
  });

  // toggle tabs
  Drupal.Sweaver.container = Drupal.Sweaver.activeTab.substr(4, Drupal.Sweaver.activeTab.length - 4);
  $('#sweaver-tabs .tab a').click(function(){
    var container = $(this).parent().attr('id').replace('tab-', '');
    if (container != Drupal.Sweaver.container) {
      if (Drupal.Sweaver.open == 'false') {
        $('#sweaver').css("height", 'auto');
        Drupal.Sweaver.open = 'true';
	    }
			$(this).parent().siblings().removeClass('active-tab');
			$(this).parent().toggleClass('active-tab');
			$('#'+ container + ' > div').show();
			$('#'+ Drupal.Sweaver.container + ' > div').hide();
			Drupal.Sweaver.container = container;
		} 
    else {
      if (Drupal.Sweaver.open == 'false') {
        $(this).parent().siblings().removeClass('active-tab');
        //$(this).parent().toggleClass('active-tab');
        $('#sweaver').css("height", 'auto');
        Drupal.Sweaver.open = 'true';
      }
	  }
    Drupal.Sweaver.activeTab =  $(this).parent().attr('id');
    Drupal.Sweaver.cookie('sweaver_open', Drupal.Sweaver.open);
    Drupal.Sweaver.cookie('sweaver_active_tab', Drupal.Sweaver.activeTab);
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
  
  // Move editor - this works, but is probably not THAT interesting :)
  /*$('.tab-move a').mousedown(function() {
	  var height = $('#sweaver').outerHeight();
	  console.log(Drupal.Sweaver.cookie('sweaver_height'));
    $('#sweaver').addClass('draggable');
	$('#sweaver').draggable();
	$('#sweaver').css('height', ''+ height +'px');
  });*/
});

/**
 * Separate switch tab function. Takes the tab as arguments and the ID's
 * of the containers will be derived from the tabs.
 */
Drupal.Sweaver.switchTab = function (remove_tab, show_tab) {
  var container_remove = remove_tab.replace('tab-', '');
  var container_show = show_tab.replace('tab-', '');		

  $('#'+ remove_tab).removeClass('active-tab');
  $('#'+ show_tab).toggleClass('active-tab');
  $('#'+ container_remove + ' > div').hide();
  $('#'+ container_show + ' > div').show();
  Drupal.Sweaver.container = container_show;
	
  Drupal.Sweaver.activeTab = show_tab;
  Drupal.Sweaver.cookie('sweaver_active_tab', show_tab);
}

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

/**
 * Display a fullscreen popup.
 */
Drupal.Sweaver.popup = '';
Drupal.Sweaver.showPopup = function(message) {
  // Close the previous message - if any.
  if (Drupal.Sweaver.popup != '') {
    $(Drupal.Sweaver.popup).hide();
  }
  
  // Create popup.
  popup = $('#sweaver-popup');
  popupBorder = 7;
  popupTop = popupBorder;
  popupWidth = $(window).width() - (popupBorder * 2) - parseInt(popup.css('padding-left')) - parseInt(popup.css('padding-right'));
  popupHeight = $(window).height() - $('#sweaver').outerHeight() - $('#sweaver-tabs').outerHeight() - (popupBorder * 2) - parseInt(popup.css('padding-top')) - parseInt(popup.css('padding-bottom'));
  $('.content', popup).css({'height' : popupHeight, 'width' : popupWidth});
  $(message).show();
  Drupal.Sweaver.popup = message;
  popup.css({'left' : popupBorder, 'top' : popupTop}).fadeIn('fast');
  $('.close', popup).click(function(){
    $(message).hide();
    Drupal.Sweaver.hidePopup();
  });
}

/**
 * Close the popup and return .
 */
Drupal.Sweaver.hidePopup = function() {
  popup = $('#sweaver-popup');
  popup.hide();
}

/**
 * Set behaviors on link which will open the popup.
 */
Drupal.behaviors.sweaverOpenPopup = {
  attach: function (context, settings) {

    $('#sweaver .popup-link a', context).each(function() {

      $(this).click(function() {

        var wrapper = $(this).attr('id').replace('link', 'data');
      
        popup = $('#sweaver-popup');
        if (popup.is(':visible') && $(this).hasClass('open-tab')) {
          Drupal.Sweaver.hidePopup();
          $(this).removeClass('open-tab');
        }
        else {
          $('#sweaver .open-tab').removeClass('open-tab');
          $(this).addClass('open-tab');
          Drupal.Sweaver.showPopup($('#'+ wrapper));
        }
        return false;
      });
    });

    $('#sweaver .form-submit').click(function() {
      Drupal.Sweaver.hidePopup();
    })
  }
}	

/**
 * Cookie plugin
 *
 * Copyright (c) 2006 Klaus Hartl (stilbuero.de)
 * Dual licensed under the MIT and GPL licenses:
 * http://www.opensource.org/licenses/mit-license.php
 * http://www.gnu.org/licenses/gpl.html
 *
 */

/**
 * Create a cookie with the given name and value and other optional parameters.
 *
 * @example $.cookie('the_cookie', 'the_value');
 * @desc Set the value of a cookie.
 * @example $.cookie('the_cookie', 'the_value', { expires: 7, path: '/', domain: 'jquery.com', secure: true });
 * @desc Create a cookie with all available options.
 * @example $.cookie('the_cookie', 'the_value');
 * @desc Create a session cookie.
 * @example $.cookie('the_cookie', null);
 * @desc Delete a cookie by passing null as value. Keep in mind that you have to use the same path and domain
 *       used when the cookie was set.
 *
 * @param String name The name of the cookie.
 * @param String value The value of the cookie.
 * @param Object options An object literal containing key/value pairs to provide optional cookie attributes.
 * @option Number|Date expires Either an integer specifying the expiration date from now on in days or a Date object.
 *                             If a negative value is specified (e.g. a date in the past), the cookie will be deleted.
 *                             If set to null or omitted, the cookie will be a session cookie and will not be retained
 *                             when the the browser exits.
 * @option String path The value of the path atribute of the cookie (default: path of page that created the cookie).
 * @option String domain The value of the domain attribute of the cookie (default: domain of page that created the cookie).
 * @option Boolean secure If true, the secure attribute of the cookie will be set and the cookie transmission will
 *                        require a secure protocol (like HTTPS).
 * @type undefined
 *
 * @name $.cookie
 * @cat Plugins/Cookie
 * @author Klaus Hartl/klaus.hartl@stilbuero.de
 */

/**
 * Get the value of a cookie with the given name.
 *
 * @example $.cookie('the_cookie');
 * @desc Get the value of a cookie.
 *
 * @param String name The name of the cookie.
 * @return The value of the cookie.
 * @type String
 *
 * @name $.cookie
 * @cat Plugins/Cookie
 * @author Klaus Hartl/klaus.hartl@stilbuero.de
 */
Drupal.Sweaver.cookie = function(name, value, options) {
    if (typeof value != 'undefined') { // name and value given, set cookie
        options = options || {};
        if (value === null) {
            value = '';
            options.expires = -1;
        }
        var expires = '';
        if (options.expires && (typeof options.expires == 'number' || options.expires.toUTCString)) {
            var date;
            if (typeof options.expires == 'number') {
                date = new Date();
                date.setTime(date.getTime() + (options.expires * 24 * 60 * 60 * 1000));
            } else {
                date = options.expires;
            }
            expires = '; expires=' + date.toUTCString(); // use expires attribute, max-age is not supported by IE
        }
        // CAUTION: Needed to parenthesize options.path and options.domain
        // in the following expressions, otherwise they evaluate to undefined
        // in the packed version for some reason...
        var path = options.path ? '; path=' + (options.path) : '';
        var domain = options.domain ? '; domain=' + (options.domain) : '';
        var secure = options.secure ? '; secure' : '';
        document.cookie = [name, '=', encodeURIComponent(value), expires, path, domain, secure].join('');
    } else { // only name given, get cookie
        var cookieValue = null;
        if (document.cookie && document.cookie != '') {
            var cookies = document.cookie.split(';');
            for (var i = 0; i < cookies.length; i++) {
                var cookie = jQuery.trim(cookies[i]);
                // Does this cookie string begin with the name we want?
                if (cookie.substring(0, name.length + 1) == (name + '=')) {
                    cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
                    break;
                }
            }
        }
        return cookieValue;
    }
};

})(jQuery);