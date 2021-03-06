'use strict';

function updateStatus($value) {
    var data = {'domainID': $value.attr('data-id')};

    $.ajax({
        type: "POST",
        url: '/ajax/domains/domain/processDomainStatus',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                if(response.status) {
                    $value.removeClass('circle_question_mark').addClass('circle_ok').css('color', '#53A93F');
                } else {
                    $value.removeClass('circle_question_mark').addClass('circle_exclamation_mark').css('color', '#E04A3F');
                }
            } else {
                $value.removeClass('circle_question_mark').addClass('circle_exclamation_mark').css('color', '#E04A3F');
            }
        })
        .fail(function(xhr) {
            if(!xhr.getAllResponseHeaders()) {
                console.log('Hit');
                return;
            }
            
            // Tell user error
            $value.removeClass('circle_question_mark').addClass('circle_exclamation_mark').css('color', '#E04A3F');
        });
}

$(document).ready(function(){
    $('.domainStatusIcon').each(function(index, value){
        var $value = $(value);
        updateStatus($value);
    });
});