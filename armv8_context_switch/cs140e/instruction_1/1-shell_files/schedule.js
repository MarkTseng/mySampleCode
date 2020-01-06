var _MS_PER_DAY = 1000 * 60 * 60 * 24;
var table = $("tbody#schedule-table");

// From http://stackoverflow.com/a/15289883
// a and b are javascript Date objects
function dateDiffInDays(a, b) {
  // Discard the time and time-zone information.
  var utc1 = Date.UTC(a.getFullYear(), a.getMonth(), a.getDate());
  var utc2 = Date.UTC(b.getFullYear(), b.getMonth(), b.getDate());
  return Math.floor((utc2 - utc1) / _MS_PER_DAY);
}

if (table && !!table.data("start")) {
  var startDate = new Date(table.data("start"));
  var now = new Date();
  var weekNumber = Math.floor(dateDiffInDays(startDate, now) / 7);

  var tableRows = table.children();
  if (weekNumber < tableRows.length) {
    var currentWeekrow = tableRows[weekNumber];
    $(currentWeekrow).addClass("info");
  }
}
