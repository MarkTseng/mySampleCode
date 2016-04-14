$(function() {

//facebook
$('#socialbuttons .facebook_like').socialbutton('facebook_like', {
	button:'standard',
	url:'http://www.html5-memo.com',
	locale:"zh_cn"
}).width(115);

$('#socialbuttons .facebook_like1').socialbutton('facebook_like', {
	button:'button_count',
	url:'http://www.html5-memo.com',
	locale:"zh_cn"
}).width(115);


$('#socialbuttons .facebook_like2').socialbutton('facebook_like', {
	button:'box_count',
	url:'http://www.html5-memo.com',
	locale:"zh_cn"
}).width(115);


$('#socialbuttons .facebook_like3').socialbutton('facebook_like', {
	button:'button_count',
	url:'http://www.html5-memo.com',
	locale:'zh_cn'
}).width(115);

$('#socialbuttons .facebook_like4').socialbutton('facebook_like', {
	button:'button_count',
	url:'http://www.html5-memo.com',
	locale:'zh_cn'
}).width(115);




//twitter

$('#socialbuttons .twitter').socialbutton('twitter', {
	button:'vertical',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_cn',
	via:'html5memo'
}).width(105);

$('#socialbuttons .twitter1').socialbutton('twitter', {
	button:'horizontal',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_cn',
	via:'html5memo'
}).width(105);

$('#socialbuttons .twitter2').socialbutton('twitter', {
	button:'none',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_cn',
	via:'html5memo'
}).width(105);

$('#socialbuttons .twitter3').socialbutton('twitter', {
	button:'horizontal',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:"zh_cnP",
	via:'html5memo'
}).width(105);

$('#socialbuttons .twitter4').socialbutton('twitter', {
	button:'horizontal',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_cn',
	via:'html5memo'
}).width(105);









//google

$('#socialbuttons .google').socialbutton('google_plusone', {
	size:"small",
	url:'http://www.html5-memo.com',
	count:true
}).width(90);

$('#socialbuttons .google1').socialbutton('google_plusone', {
	size:"medium",
	url:'http://www.html5-memo.com',
	count:true
}).width(90);

$('#socialbuttons .google2').socialbutton('google_plusone', {
	size:"standard",
	url:'http://www.html5-memo.com',
	count:true
}).width(90);

$('#socialbuttons .google3').socialbutton('google_plusone', {
	size:"tall",
	url:'http://www.html5-memo.com',
	count:true
}).width(90);

$('#socialbuttons .google4').socialbutton('google_plusone', {
	size:"medium",
	url:'http://www.html5-memo.com',
	count:false
}).width(90);







//hatena
$('#socialbuttons .hatena').socialbutton('hatena', {
    button:'standard',
    url:'http://www.html5-memo.com',
}).width(70);
$('#socialbuttons .hatena1').socialbutton('hatena', {
    button:'standard-balloon',
    url:'http://www.html5-memo.com',
}).width(70);
$('#socialbuttons .hatena2').socialbutton('hatena', {
    button:'vertical',
    url:'http://www.html5-memo.com',
}).width(70);
$('#socialbuttons .hatena3').socialbutton('hatena', {
    button:'simple',
    url:'http://www.html5-memo.com',
}).width(70);



//pinterest
$('#socialbuttons .pinterest').socialbutton('pinterest', {
	button:'vertical',
	media: 'http://cruzin.jp/zoo/img/03.jpg',
	url:'http://cruzin.jp/zoo/',
	description: 'Photo by Yutaka Hayashi'
}).width(100);
$('#socialbuttons .pinterest1').socialbutton('pinterest', {
	button:'horizontal',
	media: 'http://cruzin.jp/zoo/img/03.jpg',
	url:'http://cruzin.jp/zoo/',
	description: 'Photo by Yutaka Hayashi'
}).width(100);
$('#socialbuttons .pinterest2').socialbutton('pinterest', {
	button:'none',
	media: 'http://cruzin.jp/zoo/img/03.jpg',
	url:'http://cruzin.jp/zoo/',
	description: 'Photo by Yutaka Hayashi'
}).width(100);

});