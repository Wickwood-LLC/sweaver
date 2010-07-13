// $Id$

/**
 * Add the sweaver bar at the bottom of the theme
 */

Drupal.Sweaver = Drupal.Sweaver || {};

Drupal.Sweaver.types = new Array(); // A type groups different properties
Drupal.Sweaver.properties = new Array(); // The actual css properties
Drupal.Sweaver.selectors = new Array(); // The list of defined selectors
Drupal.Sweaver.css = new Object(); // Object with all targets and their properties
Drupal.Sweaver.path = new Array(); // Full path to the root of the document
Drupal.Sweaver.pathIndexes = new Array(); // An array with the indexes of all selected items
Drupal.Sweaver.activePath = ''; // Currently active path
Drupal.Sweaver.activeElement = new Object(); // Currently active element
Drupal.Sweaver.updateMode = true; // should the form updates be saved in css?

/**
 * Hook onload behavior
 */
$(document).ready(function() {

  Drupal.Sweaver.init();

  Drupal.Sweaver.writeCss();

  Drupal.Sweaver.addSliders();

  Drupal.Sweaver.addColorPicker();

  Drupal.Sweaver.updateForm();

  Drupal.Sweaver.bindClicks();
  
});

/**
 * Implementation of HOOK_updateCss().
 * 
 * Return editor css.
 */
// JYVE - LOOK AT the check op if (css != '') eigenlijk gaat die altijd er zijn, dus hoeft hier niet te zijn
// Daarom dat er dus af en toe lege selectors zijn - nie schoon eh ..
function sweaver_plugin_editor_updateCss() {
  var css = '';
  var fullCss = '';

  for (var key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
    css += key + '{\n';
    for(var prop in target) {
      if (Drupal.Sweaver.properties[prop]) {
        css += prop + ': ' + Drupal.Sweaver.properties[prop].prefix + target[prop] + Drupal.Sweaver.properties[prop].suffix + ';\n';
      }
    }
    if (css != '') {
      css += '}\n';
      fullCss += css;
      css = '';
    }
  }
  
  // Store css in hidden field in save form
  $("#sweaver #edit-css").val($.toJSON(Drupal.Sweaver.css));

  // Add inline css
  $("#sweaver-form #edit-css").val(fullCss);  
  
  return fullCss;
}

/**
 * Initialize member variables and properties.
 */
Drupal.Sweaver.init = function() {

  // Add sweaver class for extra margin at bottom.
  $('body').addClass('sweaver');

  // Get previously stored information or create empty object with all targets
  db_css = $("#edit-css");

  if (db_css.val() && db_css.val() != '[]'){
    Drupal.Sweaver.css = $.evalJSON(db_css.val());
    db_css.val('');
  }

  // Sweaver selectors.
  Drupal.Sweaver.selectors = Drupal.settings.sweaver['selectors'];

  // Sweaver types.
  Drupal.Sweaver.types = Drupal.settings.sweaver['types'];

  // Sweaver properties.
  Drupal.Sweaver.properties = Drupal.settings.sweaver['properties'];

  // Add div to store inline css and fill it up
  $('body > div:first').before('<div id="sweaver-css"></div>');

  // Divide the header region into two areas
  $('#sweaver_plugin_editor .sweaver-header').after('<div id="full-path" class="clear-block"></div><div id="selected-path" class="clear-block"></div>');  

  // Add a link to be able to follow links.
  $('body').append('<a href="" id="follow-link">' + Drupal.t('Click here to follow this link') + '</div>');
}

/**
 * Get all values and update the form
 */
Drupal.Sweaver.updateForm = function() {

  // Empty form values and hide unnecessary fields
  Drupal.Sweaver.initForm();
  
  // Prevent changes from being saved
  Drupal.Sweaver.updateMode = false;
  // Update form with saved settings
  if (Drupal.Sweaver.activePath != '') {
    if ($("#tab-sweaver_plugin_editor").hasClass('active-tab')) {
      $("#sweaver_plugin_editor #editor-styler").show();
    }
    var target = '';
    if (!isEmpty(Drupal.Sweaver.activeElement)) {
      var type = Drupal.Sweaver.activeElement.type;
      $.each(Drupal.Sweaver.types[type], function (index, object){
        var value = $(Drupal.Sweaver.activePath).css(object);
        if(!isEmpty(Drupal.Sweaver.properties[object]) && Drupal.Sweaver.properties[object].type == 'color') {
          $('#' + object + ' .colorSelector div').css('backgroundColor', value);
        } 
        else {
          if (value) {
            $("#sweaver_plugin_editor #edit-" + object).val(value.replace('px', ''));
          }
        }
      });
    }
    Drupal.Sweaver.updateSliders();
  }
  Drupal.Sweaver.updateMode = true;
}


/**
 * Empty form values and hide unnecessary fields
 */
Drupal.Sweaver.initForm = function() {

  Drupal.Sweaver.hideOverlays();

  if (!isEmpty(Drupal.Sweaver.activeElement)) {
    var type = Drupal.Sweaver.activeElement.type;
/*
    // Empty form values so that they are not copied over when switching
    $('#sweaver input, #sweaver #sweaver_plugin_editor select').each(function () {
      $(this).val('');
    });
    $('#sweaver .colorSelector div').css('backgroundColor', '#fff');
    $('#sweaver .slider-value').val('');
*/
    // First hide all form elements
    $('#sweaver #sweaver_plugin_editor .form-item, #sweaver #sweaver_plugin_editor .slider-wrapper').each(function (i) {
      $(this).hide();
    });

    // Show only the fields defined in the options array
    if (type && Drupal.Sweaver.types[type]) {
      $.each(Drupal.Sweaver.types[type], function (index, object){
        //$('#sweaver #edit-' + object + '-wrapper').show();
        $('#sweaver #edit-' + object + '-wrapper').show();        
      });
    }
  }
}

/**
 * Show colorPicker and hook events to it
 */
Drupal.Sweaver.addColorPicker = function() {
  $('#sweaver .colorSelector').each(function() {
    var object = $(this);
    var property = object.parent().attr('id');
    object.ColorPicker({
      color: '#ffffff',
      onShow: function (colpkr) {
        $(colpkr).fadeIn(500);
        Drupal.Sweaver.hideOverlays();        
        return false;
      },
      onHide: function (colpkr) {
        $(colpkr).fadeOut(500);
        return false;
      },
      onChange: function (hsb, hex, rgb) {
        $('div', object).css('backgroundColor', '#' + hex);
        if (Drupal.Sweaver.updateMode) {
          Drupal.Sweaver.setValue(property, hex);
        }
      }
    });
  });
}

/*
 * Add sliders through jQuery UI
 */
Drupal.Sweaver.addSliders = function() {
  $("#sweaver .slider-value").each(function() {
    $(this).after('<div class="slider-wrapper"><div id="' + $(this).attr('id').substr(5, $(this).attr('id').length - 5) + '-slider" class="slider"></div></div>');
  });

  $("#sweaver .slider-value").click(function() {  
    $(this).siblings('.slider-wrapper').children().slider("moveTo", $(this).val());
  });

  $("#sweaver .slider").each(function() {
    minSlider = Drupal.Sweaver.properties[$(this).attr('id').replace('-slider', '')].slider_min;
	  if (minSlider == null) {
	    minSlider = 0;
	  }
    maxSlider = Drupal.Sweaver.properties[$(this).attr('id').replace('-slider', '')].slider_max;
    if (maxSlider == null) {
      maxSlider = 2000;
    }
	  $(this).slider({
	    min: minSlider,
	    max: maxSlider,
	    slide: function(event, ui) {
	      if (ui.value > 1) {
	        $('#edit-' + $(this).attr("id").replace('-slider','')).val(ui.value);
	        // TODO: can I trigger onchange event here?
	        if (Drupal.Sweaver.updateMode) {
	          Drupal.Sweaver.setValue($(this).attr("id").replace('-slider',''), ui.value);
	        }
	      } else {
	        $('#' + $(this).attr("id").replace('-slider','')).val('default');
	      }
	    }
	  });
	});
}

/*
 * Update sliders value
 */
Drupal.Sweaver.updateSliders = function() {
  $('#sweaver .slider').each(function() {
    var object = $(this);
    name = object.attr('id').substr(0, object.attr('id').length - 7);
    val = $("#edit-" + name).val() == '' ? 0 : $("#edit-" + name).val();
    $(object).slider("moveTo", val);
  });
}

/**
 * Loop through all clickable area's and bind click
 */
Drupal.Sweaver.bindClicks = function() {

  // Build a list of selectors to exclude.
  var excludesArray = new Array();;
  $.each(Drupal.settings.sweaver['exclude_selectors'], function(index, object) {
    excludesArray.push(object);
  });  
  var excludes = excludesArray.join(', ');

  // Never select the editor form as themeable.
  $(excludes).click(function(event) {
    event.stopPropagation();
  });
  
  // Catch click on a selector.
  $.each(Drupal.Sweaver.selectors, function (index, object) {
    $(object.selector).filter(':parents(' + excludes + '):not(' + excludes + ')').click(function(event) {
      event.stopPropagation();
      // Only do something when the content area is visible.
      if (Drupal.Sweaver.open) { 
	      $('#sweaver_plugin_editor .sweaver-header').html('');
	      
	      // handle clicking on a link.
	      $('#follow-link').hide();
	      if(object.selector == 'a' && $(this).attr('id') != 'follow-link') {
	        var position = $(this).offset();
	        $('#follow-link').attr('href', $(this).attr('href')).css({'top' : position.top + $(this).height() + 5, 'left': position.left}).fadeIn();
	        event.preventDefault();
	      }
	
	      if ($(this).attr('id') != 'follow-link') {
	        // Reset some values.
	        Drupal.Sweaver.path.length = 0;
	        Drupal.Sweaver.pathIndexes.length = 0;
	        $("#selected-path").html('');
	        $("#full-path").html('');        
	
	        // Build path with parents.
	        Drupal.Sweaver.buildPath(this);
	
	        Drupal.Sweaver.updateForm();
	      }
      }
    });
  });

  // Hide sliders and close groups when clicking outside of them.
  $("#sweaver").click(function() {
    Drupal.Sweaver.hideOverlays();
  });

  // Update css when something is changed in the form.
  $("#sweaver_plugin_editor input[id^=edit-], #sweaver_plugin_editor select[id^=edit-]").change(function(){
    if (Drupal.Sweaver.updateMode) {
      Drupal.Sweaver.setValue($(this).attr('name'), $(this).val());
    }
  });

  // Show the slider when a numeric value is entered.
  $("#sweaver_plugin_editor  .slider-value").click(function(event){
    event.stopPropagation();
    slider = $(this).siblings('.slider-wrapper');
    
    if (slider.is(':visible')) {
      // Close slider again on second click.
      slider.css({'visibility' : 'hidden'});
    }
    else {
	    // Hide all other sliders.
	    $('#sweaver_plugin_editor .slider-wrapper').css({'visibility' : 'hidden'});
	
	    var left = slider.siblings('label').width() - 10;
	    var top = slider.parent().outerHeight();
	    slider.css({'left' : left, 'top' : top}).css({'visibility' : 'visible'});
	  }
  });
}

/**
 * Store the parents of a clicked item.
 */
Drupal.Sweaver.buildPath = function(object) {
  var index = 0;

  // Collect info on currently active item.
  Drupal.Sweaver.activeElement.id = $(object).attr('id');
  Drupal.Sweaver.activeElement.class = $(object).attr('class').split(' ');
  Drupal.Sweaver.activeElement.tag = $(object).get(0).tagName.toLowerCase();
  Drupal.Sweaver.activeElement.type = $(object).css('display');
  Drupal.Sweaver.activeElement.object = object;

  // Add active element to first element in the path array.
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'class' : Drupal.Sweaver.activeElement.class, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'object' : Drupal.Sweaver.activeElement.object});
  
  // Show the currenty active path and the full path.
  Drupal.Sweaver.addToFullPath(object, index, true);
  Drupal.Sweaver.printActivePath(0, object);

  // Traverse all parents and save them in the path array.
  var i = 1;
  var active;
  $(object).parents().each(function() {
    active = false;
    
	  id = $(this).attr('id');
	  class = $(this).attr('class').split(' ');
	  tag = $(this).get(0).tagName.toLowerCase();
    type = $(this).css('display');	  
	  item = this;
    Drupal.Sweaver.path[i] = new Object({'id' : id, 'class' : class, 'tag' : tag, 'type' : type, 'object' : item});	
    
    // If selector is tagged as 'highlight', automatically select it.  
    $.each(Drupal.Sweaver.selectors, function (index, selector) {
      if (selector.highlight == '1' && (selector.selector == '#' + id || selector.selector == '.' + class || selector.selector == tag)) {
        active = true; 
        Drupal.Sweaver.printActivePath(i, item);        
      }
    }); 
    
    // Add all items to the full path except for the html tag.
    if (tag != 'html') {
      Drupal.Sweaver.addToFullPath(item, i, active);
    }
    
    i++;
  });

  $("#full-path").prepend('<span class="label">' + Drupal.t('Full path: ') + '</span>');
}

/**
 * Print active path.
 */
Drupal.Sweaver.addToFullPath = function(object, index, active) {
  var path_separator = ' > ';
  var active_class = '';

  // Don't show a seperator after the last item in the path.
  if (index == 0) {
    path_separator = '';
  }

  // Add an active class to the selected items.
  if (active == true) {
    active_class = ' class="active"';
  }

  // Populate path with clickable links.
  $("#full-path").prepend($('<span id="thid-' + index + '" '+ active_class +'><a href="#">' + Drupal.Sweaver.objectToReadable(Drupal.Sweaver.path[index]) + '</a> '+ path_separator +' </span>').click(function() {
    $(this).toggleClass('active');
    Drupal.Sweaver.printActivePath(index, $(object));

	  // Reset the active element as it might have changed.
	  Drupal.Sweaver.pathIndexes.sort(function(a,b){return a - b});
	  Drupal.Sweaver.activeElement = Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[0]];
	  Drupal.Sweaver.updateForm(); 
    
    // Stop the link from doing anything.  
    return false;
  }));
}

/**
 * Print the full path.
 */
Drupal.Sweaver.printActivePath = function(i, item) {
  // Reset the previous path and add the nex item to pathIndexes.
  $("#selected-path").html('');

  // Do not add the item when selected or remove it from Active path.
  var position = $.inArray(i, Drupal.Sweaver.pathIndexes);
  if (position < 0) {
    Drupal.Sweaver.pathIndexes.unshift(i);
  }
  else {
    // Remove from pathIndexes if necessary.
    for (var key in Drupal.Sweaver.pathIndexes) {
      if (Drupal.Sweaver.pathIndexes[key] == i) {
        Drupal.Sweaver.pathIndexes.splice(key, 1);
      }
    }
  }

  // Sort pathIndexes.
  Drupal.Sweaver.pathIndexes.sort(function(a,b){return a - b});


  // Print the selected path in human-readable language.
  for ( var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i ){
    if (i > 0) {
      $("#selected-path").append(' in ');
    }
    $("#selected-path").append(Drupal.Sweaver.objectToReadable(Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]]));
  }
  $("#selected-path").prepend('<span class="label">' + Drupal.t('Selected item: ') + '</span>');

  // Save the currently active css path.
  Drupal.Sweaver.activePath = '';
  Drupal.Sweaver.pathIndexes.reverse();
  for (var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i){
    Drupal.Sweaver.activePath += Drupal.Sweaver.objectToCss(Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]]) + ' ';
  }
}

/**
 * Store new value and update inline css.
 */
Drupal.Sweaver.setValue = function(property, value) {
  if (!Drupal.Sweaver.css[Drupal.Sweaver.activePath]) {
    Drupal.Sweaver.css[Drupal.Sweaver.activePath] = new Object();
  }
  Drupal.Sweaver.css[Drupal.Sweaver.activePath][property] = value;
  Drupal.Sweaver.writeCss();
}

// JYVE REWORK ME :) Mischien eerder naar 1 button die 'show history' doet en dan 'hide history' ?
// code rond history & styler is in sweaver_plugin_editor.inc (vanaf lijn 154 in form & in render)
// Kijk ook eens naar de 2 functies hieronder (writeHistory) - deletePropertyFromHistory
// Vraag is ook, history is misschien slechte naam, want da impliceert kunnen undo & redo, terwijl
// ge enkel kunt delete - dus mss eerder hernoemen naar 'current selections' ofzo?
Drupal.Sweaver.editortab = 'editor-styler';
Drupal.behaviors.editorTabToggle = function(context) {
  $('.editor-switcher a').click(function(){
	if (Drupal.Sweaver.editortab == 'editor-styler') {
      $('#editor-styler').hide();
      $('#editor-history').show();
      Drupal.Sweaver.editortab =  'editor-history';
      Drupal.Sweaver.writeHistory();
    }
    else {
      $('#editor-styler').show();
      $('#editor-history').hide();
      Drupal.Sweaver.editortab =  'editor-styler';
    }
  });
}

/**
 * Write history.
 */
Drupal.Sweaver.writeHistory = function() {
	
  var history = '';
  for (var key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
	for (var prop in target) {
      if (Drupal.Sweaver.properties[prop]) {
        history += '<div>';
        history += '<a href="javascript:Drupal.Sweaver.deleteProperty(\''+ key +'\', \''+ prop +'\');">'+ Drupal.t('Delete') +'</a> ';
        history += key + ': '+ prop + ': ' + Drupal.Sweaver.properties[prop].prefix + target[prop] + Drupal.Sweaver.properties[prop].suffix + ';';
        history += '</div>';
	  }
	}
  }
  
  $('#editor-history-list').html(history);	
}

/**
 * Delete a property from a selector.
 */
Drupal.Sweaver.deleteProperty = function(key, property) {

  var target = Drupal.Sweaver.css[key];
  Drupal.Sweaver.css[key] = {};
  for (var prop in target) {
    if (prop != property) {
      Drupal.Sweaver.css[key][prop] = target[prop];
    }
  }
  
  Drupal.Sweaver.writeCss();
  Drupal.Sweaver.writeHistory();
}

/**
 * Translate an parent item in a human-readable name.
 */
Drupal.Sweaver.objectToReadable = function(object) {

  var translation = '';
  
  // special cases
  if (object.tag == 'body') {
    translation = Drupal.Sweaver.selectors.body.description;
  } else {
	  if (object.id) {
	    if (Drupal.Sweaver.selectors[object.id]){
	      translation = Drupal.Sweaver.selectors[object.id].description;
	    } 
	    else {
	      translation = 'the ' + object.id + ' region';
	    }
	  } 
	  else if (object.class[0]) {
	    if (Drupal.Sweaver.selectors[object.class]){
	      translation = Drupal.Sweaver.selectors[object.class].description;
	    }   
	    else {
	      translation = 'all ' + object.class[0];
	    }
	  } 
	  else if (object.tag) {
		  $.each(Drupal.Sweaver.selectors, function() {
		    if (this.selector == object.tag) {
		      translation = this.description;
		    }
		  });
		  if (translation == '') {
		    translation = object.tag;
		  }
	  }
  }
  return translation;
}

/**
 * Translate an parent item in a css name.
 */
Drupal.Sweaver.objectToCss = function(object) {
  if (object.id) {
    return '#' + object.id;
  }
  else if (object.class[0]) {
    return '.' + object.class[0];
  }
  else if (object.tag) {
    return object.tag;
  }
}

Drupal.Sweaver.hideOverlays = function() {
  // Hide all sliders.
  $('#sweaver .slider-wrapper').css({'visibility' : 'hidden'});
   
  // Hide all groups.
  $('#sweaver .sweaver-group-active .sweaver-group-content').css({'visibility' : 'hidden', 'right' :  '-10000px'}); 
  $('#sweaver .sweaver-group-active').removeClass('sweaver-group-active');
}

Drupal.behaviors.openGroup = function() {

  // Open a group when it is clicked.
  $('#sweaver .sweaver-group').click(function (event) {
    event.stopPropagation();  
    Drupal.Sweaver.hideOverlays(); 
    
    var content = $('.sweaver-group-content', this);
    if(!content.is(':visible')) {
      
      // Open the clicked group and make sure it does not cover the label or fall off the screen.
      group = $(this);
      var right = 0;
      var top = group.outerHeight() - 10;
      var position = group.offset();
      if (position.top + content.outerHeight() > $(window).height()) {
        top = - content.height();
      }
      content.css({'right' : right, 'top' : top, 'visibility' : 'visible'}).parent().toggleClass('sweaver-group-active');
    }
  });
}


/**
 * Add custom expression to exclude all selectors in the sweaver bar.
 */
$.expr[':'].parents = function(a,i,m){
  return jQuery(a).parents(m[3]).length < 1;
};

/**
 * Helper function to check if an object is empty.
 */
function isEmpty(obj) {
  for(var prop in obj) {
    if(obj.hasOwnProperty(prop))
      return false;
  }
  return true;
}