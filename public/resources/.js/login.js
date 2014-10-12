'use strict';

function bindLoginEvent() {
    $('#loginForm').on('submit', function (e) {
        e.preventDefault();

        $('#loginForm').children('button').attr('disabled', 'disabled');
        $('#alertBox').slideUp().html(bootstrapAlert('info', 'Sending login request.')).slideDown();

        var inputs = [
            'loginEmail',
            'loginPassword',
        ];
        var data = {};
        var submitThis = true;

        inputs.forEach(function(value){
            switch (checkFormElement('#' + value)) {
                case true:
                    data[value] = getInputValue('#' + value);
                    break;
                case -1:
                    // Skip value
                    break;
                default:
                    submitThis = false;
                    break;
            }
        });

        if(submitThis) {
            doLoginAJAX(data);
        } else {
            $('#alertBox').html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
            $('#loginForm').children('button').removeAttr('disabled');
        }
    });
}

function doLoginAJAX(data){
        var xhr = $.ajax({
            type: "POST",
            url: '/ajax/login/processLogin',
            data: data
        }).done(function (response) {
                if (typeof(response.errors) != 'undefined') {
                    // Tell user is processing
                    if (response.errors.length == 0) {
                        if(response['user']['loggedIn'] == 1) {
                            $('#alertBox').html(bootstrapAlert('success', 'Logging you in now, ' + response.user.f + ' ' + response.user.l + '.')).slideDown();
                            document.location.href = '/home';
                        }
                    } else {
                        if (response.errors.e5 || response.errors.e6 || response.errors.e7) {
                            $('#alertBox').html(bootstrapAlert('warning', 'We could not find any user with these details.')).slideDown();
                            $('#loginForm').children('button').removeAttr('disabled');
                        }  else {
                          // Tell user error
                          $('#alertBox').html(bootstrapAlert('danger', 'Login request failed, please try again.')).slideDown();
                          $('#loginForm').children('button').removeAttr('disabled');
                        }
                    }
                } else {
                    $('#alertBox').html(bootstrapAlert('success', 'Logging you in now, ' + response.user.f + ' ' + response.user.l + '.')).slideDown();
                    document.location.href = '/home';
                }
        }).fail(function () {
            // Tell user error
            $('#alertBox').html(bootstrapAlert('danger', 'Login request failed, please try again.')).slideDown();
            $('#loginForm').children('button').removeAttr('disabled');
        });

}

$(document).ready(function () {
	if(Modernizr.websockets && Modernizr.fontface && Modernizr.svg && Modernizr.borderradius && Modernizr.webworkers) {
		bindLoginEvent();
	}  else {
		$('#alertBox').html(bootstrapAlert('danger', 'Your browser is unsupported.')).slideDown();
	}
});
