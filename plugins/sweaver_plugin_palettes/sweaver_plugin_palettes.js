// $Id$

/**
 * Add an extra color css
 */

Drupal.Sweaver = Drupal.Sweaver || {};

/**
 * Hook onload behavior
 */
$(document).ready(function() {
  $('#sweaver_plugin_palettes .colors').click(function(event) {
    Drupal.Sweaver.changed = true;
    
    var $this = $(this); 

    // Remove the stylesheet that was added through jQuery.
    if ($('head link#sweaver-palette').length > 0) {
      $('head link#sweaver-palette').remove();
    } 

    // Remove the stylesheet if it was already loaded in preprocess_page.
    if ($('head link[href*=' + $this.children('.file').text() + ']').length > 0) {
      $('head link[href*=' + $this.children('.file').text() + ']').remove();
    }
    
    if ($this.hasClass('active')) {
      // Remove the active class.
      $this.removeClass('active'); 
      
      // Reset the active palette.
      $('#sweaver_plugin_palettes #edit-sweaver-plugin-palette').val('');
    }
    else {
	    // Add a external stylesheet container in the head section.
	    var link = '<link id="sweaver-palette" href="' + $('.file', this).text() + '" media="all" rel="stylesheet" />';
	    $('head').append(link);
	    
      // Remove the active classes.
      $('#sweaver_plugin_palettes .active').removeClass('active');
    	    
	    // Add an active class.
	    $this.addClass('active'); 
	    
	    // Set the active palette.
      $('#sweaver_plugin_palettes #edit-sweaver-plugin-palette').val($this.children('.key').text());
	  }
  });
});