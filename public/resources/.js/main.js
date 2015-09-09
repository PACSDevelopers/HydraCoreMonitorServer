'use strict';

function formatBytes(bytes, precision) {
    var units = ['B', 'KB', 'MB', 'GB', 'TB'];

    var bytes = Math.max(bytes, 0);
    var pow = Math.floor((bytes ? Math.log(bytes) : 0) / Math.log(1024));
    pow = Math.min(pow, units.length - 1);
    bytes /= (1 << (10 * pow));

    return Math.round(bytes, precision) + ' ' + units[pow];
}

function drawHCStats() {
    if(typeof (window.performance) != 'undefined') {
        if(typeof (window.performance.timing) != 'undefined') {
            var now = new Date().getTime();
            var timing = window.performance.timing;
            var loadElement = document.getElementById('pageLoadedTime');
            var responseElement = document.getElementById('pageResponseTime');
            responseElement.innerHTML = 'Request transferred in ' + ((timing.responseEnd - timing.responseStart) / 1000).toFixed(3) + ' seconds';
            loadElement.innerHTML = 'Page loaded in ' + ((now - timing.responseEnd) / 1000).toFixed(3) + ' seconds';
        };

        if(typeof (window.performance.memory) != 'undefined') {
            var memoryElement = document.getElementById('pageMemory');
            memoryElement.innerHTML = 'Memory: ' + formatBytes(window.performance.memory.usedJSHeapSize, 2) + ' / ' + formatBytes(window.performance.memory.totalJSHeapSize, 2) + ' / ' + formatBytes(window.performance.memory.jsHeapSizeLimit, 2);
        };
    };
    var resourcesElement = document.getElementById('pageResources');
    var scripts = document.getElementsByTagName('script');
    var numberOfScripts = 0;
    for (var script in scripts) {
       if(scripts.hasOwnProperty(script)){
         if(scripts[script].src) {
           numberOfScripts++;
         }
       }
    }

    var links = document.getElementsByTagName('link');
    var numberOfStylesheets = 0;
    for (var link in links) {
       if(links.hasOwnProperty(link)){
         if(links[link].href && links[link].type == 'text/css') {
           numberOfStylesheets++;
         }
       }
    }

    var images = document.getElementsByTagName('img');
    var actualImages = {};
    for (var image in images) {
       if(images.hasOwnProperty(image)){
         if(images[image].src) {
           actualImages[images[image].src] = true;
         }
       }
    }

    var numberOfImages = 0;
    for (var image in actualImages) {
        if (actualImages.hasOwnProperty(image)) {
          numberOfImages++;
        }
    }

    resourcesElement.innerHTML = 'Resources: ' + (numberOfScripts + numberOfStylesheets + numberOfImages) + ' (' + numberOfScripts + ', ' + numberOfStylesheets + ', ' + numberOfImages + ')';
}

$.xhrPool = [];
$.xhrPool.abortAll = function() {
    $(this).each(function(idx, jqXHR) {
        jqXHR.abort();
    });
    $.xhrPool = [];
};

$.ajaxSetup({
    beforeSend: function(jqXHR) {
        $.xhrPool.push(jqXHR);
    },
    complete: function(jqXHR) {
        var index = $.inArray(jqXHR, $.xhrPool);
        if (index > -1) {
            $.xhrPool.splice(index, 1);
        }
    }
});


$(document).ready(function (){

	// Enable the polyfills that are needed
    $.webshims.polyfill();
    
    window.lastWidth = window.innerWidth;
    $(window).resize(function() {
        if(window.lastWidth != window.innerWidth) {
            window.lastWidth = window.innerWidth;
            $(this).trigger('resizeStart');
            if(this.resizeTO) clearTimeout(this.resizeTO);
            this.resizeTO = setTimeout(function() {
                $(this).trigger('resizeEnd');
            }, 500);
        }
    });
    
    $('.falseLink').on('click', function(e){
        e.preventDefault();
    });

    $('.toolTipPlease').tooltip();

    $('.toolTipIconPlease').on('mouseenter', function(){
      var $this = $(this);
      var $parent =  $this.parent();
      var $input = $('#' + $parent.attr('for'));
      $this.tooltip();
      $this.attr('data-toggle', $input.attr('data-toggle'));
      $this.attr('data-placement', $input.attr('data-placement'));
      if($input.attr('title').length) {
        $this.attr('title', $input.attr('title'));
      }
    });

    $('.nav-tabs li').each(function(index, element) {
      var $element = $(element);
      if(!$element.hasClass('active')) {
        var $href = $element.children().attr('href');
        if($href.indexOf('#') > -1) {
          $($href).addClass('hidden');
        }
      }
    })

    $('.nav-tabs a').click(function (e) {
        var $this = $(this);
        var $href = $this.attr('href');
        if($href.indexOf('#') > -1) {
          e.preventDefault();
          var $otherTabs = $this.parent().parent().children();
          $otherTabs.each(function(index, element) {
            var $element = $(element);
            $element.children().each(function(index2, element2) {
              var $element2 = $(element2);
              if($element2[0] !== $this[0]) {
                var $href = $element2.attr('href');
                if($href.indexOf('#') > -1) {
                  $($href).addClass('hidden')
                }
              }
            });
          });
          $this.tab('show');
          $($href).removeClass('hidden');
        }
    });
    
    $(document).on('keyup', '.input-force-lowercase', function(){
        var val = $(this).val();
        val = val.toLowerCase();
        $(this).val(val);
    });

    $(window).bind('beforeunload', function () {
        if($.xhrPoo) {
            $.xhrPool.abortAll();
        }
    });
    
});