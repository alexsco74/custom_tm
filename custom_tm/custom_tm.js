(function ($) {
    Drupal.behaviors.customTm = {
        attach: function (context, settings) {
            $('.custom-tm-page-content', context).each(function () {
                var contextC = $(this);
                if (!contextC.hasClass('custom-tm-check-all-process')) {
                    $(':checkbox.custom-tm-term-check-all', contextC).bind('change', function () {
                        // $(this).addClass('custom-tm-check-all-progress');
                        if ($(this).is(':checked')) {
                            $(':checkbox.custom-tm-term-check-it:not(:checked)', contextC).each(function () {
                                $(this).trigger('click');
                            });
                        } else {
                            $(':checkbox.custom-tm-term-check-it:checked', contextC).each(function () {
                                $(this).trigger('click');
                            });
                        }
                    });
                    contextC.addClass('custom-tm-check-all-process');
                }
            });
            $('form.custom-tm-auto-submit', context).each(function () {
                var thisForm = $(this);
                $('input.custom-tm-auto-submit-item, select.custom-tm-auto-submit-item', thisForm).bind('change', function () {
                    thisForm.submit();
                });
            });

            $('div.custom-tm-big-radios').once('custom-tm-big-radios', function () {
                var thisContainer = $(this).parent();
                var thisContent = $(this);
                var switchIcon = $('<span class="custom-tm-big-radios-icon"></span>');
                var thisSwitch = $('label:first', thisContainer);
                var checkedLabelText = $('label', $('input:checked', thisContent).parent()).text();
                var checkedLabel = $('<span class="custom-tm-big-radios-checked-label">' + checkedLabelText + '</span>');
                thisSwitch.addClass('custom-tm-big-radios-switch');
                thisSwitch.prepend(switchIcon).append(checkedLabel);

                function customTmBigRadiosContentClose(thisContentIn, thisSwitchIn) {
                    $('.custom-tm-big-radios-icon', thisSwitchIn).text('-');
                    thisContentIn.hide();
                    thisSwitchIn.removeClass('custom-tm-big-radios-switch-opened');
                };

                function customTmBigRadiosContentOpen(thisContentIn, thisSwitchIn) {
                    $('.custom-tm-big-radios-icon', thisSwitchIn).text('+');
                    thisContentIn.show();
                    thisSwitchIn.addClass('custom-tm-big-radios-switch-opened');
                };

                //init
                if (checkedLabelText == undefined || checkedLabelText == '') {
                    customTmBigRadiosContentOpen(thisContent, thisSwitch);
                } else {
                    customTmBigRadiosContentClose(thisContent, thisSwitch);
                }

                //event switch
                thisSwitch.bind('click', function () {
                    if (thisSwitch.hasClass('custom-tm-big-radios-switch-opened')) {
                        customTmBigRadiosContentClose(thisContent, thisSwitch);
                    } else {
                        customTmBigRadiosContentOpen(thisContent, thisSwitch);
                    }
                })

                //event change radio
                $('input:radio', thisContent).bind('click', function () {
                    $('.custom-tm-big-radios-checked-label', thisSwitch).text($('label', $('input:checked', thisContent).parent()).text());
                    customTmBigRadiosContentClose(thisContent, thisSwitch);
                });
            });
        }
    };
})(jQuery);