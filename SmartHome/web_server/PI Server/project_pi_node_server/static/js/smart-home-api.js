(function () {

	//$('#light1').on('click', function(){
	//    $.ajax({
	//        type: "GET",
	//        url: '/light1',
	//        success: function(data){
	//           console.log(data);
	//    });
	//});
	//$('#light1').on('click', function(){
	//	$.ajax({
	//	  url: "/light1",
	//	  context: document.body
	//	}).done(function() {
	//		console.log(data);
	//	});
	//});


	$('#lightson').on('click', function () {
		$.ajax({
			url: "/all-lights-on",
			context: document.body
		}).done(function () {
			console.log(e.currentTarget.id);
		});
		$(this).removeClass("btn-primary");
		$(this).addClass("btn-info");
		setTimeout(() => {
			$(this).removeClass("btn-info");
			$(this).addClass("btn-primary");
		}, 700);
	});

	$('#lightsoff').on('click', function () {
		$.ajax({
			url: "/all-lights-off",
			context: document.body
		}).done(function () {
			console.log(e.currentTarget.id);
		});
		$(this).removeClass("btn-primary");
		$(this).addClass("btn-info");
		setTimeout(() => {
			$(this).removeClass("btn-info");
			$(this).addClass("btn-primary");
		}, 700);
	});

	var btnClassClick = function (e) {
		//alert("Button clicked from class: "+e.currentTarget.id);
		$.ajax({
			url: "/harq/?value=" + e.currentTarget.id,
			context: document.body
		}).done(function () {
			console.log(e.currentTarget.id);
		});
		$(this).removeClass("btn-primary");
		$(this).addClass("btn-info");
		setTimeout(() => {
			$(this).removeClass("btn-info");
			$(this).addClass("btn-primary");
		}, 700);
	}


	var btnTimeClassClick = function (e) {
		var time = window.prompt("enter time in min for action", "1");
		$.ajax({
			url: "/req-with-at-time/?value=" + e.currentTarget.id + "&time=" + time,
			context: document.body
		}).done(function () {
			//console.log(e.currentTarget.id);
		});
	}

	$('.time').on('click', btnTimeClassClick);





	$('.btn').on('click', btnClassClick);

	$('.showrecord').click(function () {
		$('#recordForm').css({
			"display": "inline-block"
		});
	});


	$('#close').click(function () {
		$('#recordForm').css({
			"display": "none"
		});
	});

	$('.deletecmd').click(function (e) {
		$.ajax({
			url: "/deletecmd/?value=" + e.currentTarget.id,
			context: document.body
		}).done(function () {
			console.log("delete done: " + e.currentTarget.id);
			location.reload();
		});
	});



})(jQuery);