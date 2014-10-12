function validateEmail(email) {
  var emailRegex = new RegExp('(?:[a-z0-9!#$%&\'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&\'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\\])');
  return (emailRegex.test(email) > 0);
}

function validateURL(url) {
  var urlRegex = new RegExp('(http|ftp|https)://[\\w-]+(\\.[\\w-]+)+([\\w.,@?^=%&amp;:/~+#-]*[\\w@?^=%&amp;/~+#-])?');
  return (urlRegex.test(url) > 0);
}

function checkFormElement(thisElement) {

    var isSuccess = false;
    var $thisElement = $(thisElement);
    var $parent = $thisElement.parent();
    var value = getInputValue(thisElement);

    if ($thisElement.attr('data-orgval') !== undefined) {
        if($thisElement.attr('data-orgval') === value) {
          $parent.removeClass('has-error').removeClass('has-success');
          console.log('Hit 1');
          return -1;
        }
    }

    if ($thisElement.attr('data-orgvav-array') !== undefined) {
        if($thisElement.attr('data-orgvav-array') === JSON.stringify(value)) {
        $parent.removeClass('has-error').removeClass('has-success');
        console.log('Hit 2');
        return -1;
      }
    }

    if ($thisElement.attr('data-orgval-md5') !== undefined) {
        if($thisElement.attr('data-orgval-md5') === MD5(value)) {
          $parent.removeClass('has-error').removeClass('has-success');
          console.log('Hit 3');
          return -1;
        }
    }

    if (!$thisElement.length) {
        $parent.removeClass('has-error').removeClass('has-success');
        console.log('Hit 4');
        return -1;
    }

    if ($thisElement.is('[required]')) {
        if($thisElement.attr('pattern')) {
            isSuccess = (new RegExp($thisElement.attr('pattern')).test($thisElement.val()) > 0);
        } else {
            switch ($thisElement.attr('type')) {
                case 'url':
                    isSuccess = validateURL(value);
                    break;
                case 'email':
                    isSuccess = validateEmail(value);
                    break;
                case 'checkbox':
                    if ($thisElement.is(':checked')) {
                        isSuccess = true;
                    }
                    break;
                case 'datetime-local':
                    isSuccess = true;
                    var min = 0;
                    var max = 9007199254740;

                    if($thisElement.attr('min') !== undefined) {
                      min = $thisElement.attr('min');
                    }

                    if($thisElement.attr('max') !== undefined) {
                      max = $thisElement.attr('max');
                    }

                    if($thisElement.attr('data-min') !== undefined) {
                      min = $thisElement.attr('data-min');
                    }

                    if($thisElement.attr('data-max') !== undefined) {
                      max = $thisElement.attr('data-max');
                    }

                    if(value < 0 || value < min) {
                      var minDate = new Date(min * 1000)
                      minDate.setMinutes(minDate.getMinutes() + new Date().getTimezoneOffset());
                      console.log(minDate);
                      $thisElement.val(minDate.toDateTimeLocal());
                    }

                    if(value > max) {
                      var maxDate = new Date(max * 1000);
                      maxDate.setMinutes(maxDate.getMinutes() + new Date().getTimezoneOffset());
                      console.log(maxDate);
                      $thisElement.val(maxDate.toDateTimeLocal());
                    }
                break;
                default:
                    if (value && value.length) {
                        var min = parseInt($thisElement.attr('min'));
                        var max = parseInt($thisElement.attr('max'));

                        isSuccess = true;
                        if($thisElement.attr('type') == 'number') {
                          var step = parseInt($thisElement.attr('step'));
                          if(value < min) {
                              $thisElement.val(min);
                              checkFormElement(thisElement);
                          }

                          if(value > max) {
                              $thisElement.val(max);
                              checkFormElement(thisElement);
                          }

                          if(step) {
                            value = getInputValue(thisElement);
                            if(value % step !== 0) {
                              $thisElement.val(Math.floor(value));
                              checkFormElement(thisElement);
                            }

                            var tempVal = value.replace(/\.+$/, '');
                            if(tempVal != value) {
                              $thisElement.val(Math.floor(tempVal));
                              checkFormElement(thisElement);
                            }
                          }

                        } else {
                          if(value.length < min) {
                              isSuccess = false;
                          }

                          if(value.length > max) {
                              isSuccess = false;
                          }
                        }

                    }
                    break;
            }
        }
    } else {
        isSuccess = true;
    }

    if (isSuccess) {
        $parent.removeClass('has-error').addClass('has-success');
        return true;
    } else {
        $parent.removeClass('has-success').addClass('has-error');
        return false;
    }
}

function getInputValue(thisElement) {
    var $thisElement = $(thisElement);
    var tagName = $thisElement.prop('tagName');
    if(tagName === 'SELECT') {
      if($thisElement.attr('multiple')) {
        var returnArray = [];
        $thisElement.find(':selected').each(function(index, element){
          $element = $(element);
          returnArray.push($element.val());
        });
        return returnArray;
      } else {
        return $thisElement.find(':selected').val();
      }
    } else {
      if ($thisElement.attr('type') == 'checkbox') {
          return $thisElement.is(':checked');
      } else {
          if($thisElement.attr('type') == 'datetime-local') {
            var globaltime = new Date($thisElement.val());
            return (globaltime.setMinutes(globaltime.getMinutes() + globaltime.getTimezoneOffset())/1000);
          } else {
            if($thisElement.hasClass('summernote')) {
              return $thisElement.code();
            } else {
              return $thisElement.val();
            }
          }
      }
    }
}

function bindFormEvents() {
    var $allFormElements = $('.form-control, .form-checkbox');
    $allFormElements.on('change', function () {
        var formElement = this;
        setTimeout(function () {
            checkFormElement(formElement);
        }, 100);
    });

    $allFormElements.on('keyup', function () {
        var formElement = this;
        setTimeout(function () {
            checkFormElement(formElement);
        }, 100);
    });
}

$(document).ready(function () {
    bindFormEvents();
});
