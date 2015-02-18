/*
 * Copyright (C) 2014 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License, version 3
 * as published by the Free Software Foundation.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Hatohol. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var HatoholPager = function(params) {
  //
  // Possible parameters in the "params" argument:
  //
  //   numRecordsPerPage: <number> [optional]
  //     Defaut: 50
  //
  //   numTotalRecords: <number> [optional]
  //     Defaut: -1 (unknown)
  //
  //   currentPage: <number> [optional]
  //     Defaut: 0
  //
  //   maxPagesToShow: <number> [optional]
  //     Defaut: 10
  //
  //   selectPageCallback: <function> [optional]
  //     function(page)
  //     Note: Called when a page is selected or numRecordsPerPage is changed.
  //           The "page" argument will be undefined when the numRecordsPerPage
  //           is changed.
  //
  //   parentElements: <jQuery object> [optional]
  //     Default: $("ul.pagination").
  //     Note: Elements to show pager
  //
  //   numRecordsPerPageEntries: <jQuery object> [optional]
  //     Default: $("input.num-records-per-page");
  //     Note: input elements to change numRecordsPerPage
  //
  self = this;
  self.parentElements = $("ul.pagination");
  self.numRecordsPerPageEntries = $("input.num-records-per-page");
  self.numRecordsPerPage = 50;
  self.currentPage = 0;
  self.maxPagesToShow = 5;
  self.selectPageCallback = null;

  self.update(params);

  $(self.numRecordsPerPageEntries).change(function() {
    var val = parseInt(self.numRecordsPerPageEntries.val());
    if (!isFinite(val) || val < 0)
      val = self.numRecordsPerPage;
    self.numRecordsPerPageEntries.val(val);
    self.numRecordsPerPage = val;
    if (self.selectPageCallback)
      self.selectPageCallback();
  });
};

HatoholPager.prototype.getTotalNumberOfPages = function() {
  if (this.numTotalRecords < 0)
    return -1; // unknown
  if (!this.numRecordsPerPage || this.numRecordsPerPage < 0)
    return this.numTotalRecords > 0 ? 1 : 0;
  return Math.ceil(this.numTotalRecords / this.numRecordsPerPage);
};

HatoholPager.prototype.applyParams = function(params) {
  if (params && params.parentElements)
    this.parentElements = params.parentElements;
  if (params && params.numRecordsPerPageEntries)
    this.numRecordsPerPageEntries = params.numRecordsPerPageEntries;
  if (params && !isNaN(params.numRecordsPerPage))
    this.numRecordsPerPage = params.numRecordsPerPage;
  if (params && !isNaN(params.currentPage))
    this.currentPage = params.currentPage;
  if (params && !isNaN(params.numTotalRecords))
    this.numTotalRecords = params.numTotalRecords;
  if (params && !isNaN(params.maxPagesToShow))
    this.maxPagesToShow = params.maxPagesToShow;
  if (params && params.selectPageCallback)
    this.selectPageCallback = params.selectPageCallback;
  if (params && !isNaN(params.numRecords))
    this.numRecords = params.numRecords;
};

HatoholPager.prototype.getPagesRange = function(params) {
  var numPages = this.getTotalNumberOfPages();
  var firstPage, lastPage;

  firstPage = this.currentPage - Math.floor(this.maxPagesToShow / 2);
  if (firstPage + this.maxPagesToShow > numPages)
    firstPage = numPages - this.maxPagesToShow;
  if (firstPage < 0)
    firstPage = 0;

  lastPage = firstPage + this.maxPagesToShow - 1;
  if (lastPage >= numPages)
    lastPage = numPages - 1;

  return {
    firstPage: firstPage,
    lastPage:  lastPage,
  };
};

HatoholPager.prototype.createItem = function(label, enabled, getPageFunc) {
  var self = this;
  var numPages = this.getTotalNumberOfPages();
  var anchor = $("<a>", {
    href: "#",
    html: label,
    click: function () {
      var page;
      if (getPageFunc)
        page = getPageFunc();
      else
        page = parseInt($(this).text()) - 1;
      if (page < 0 || (numPages >= 0 && page >= numPages))
        return;
      if (!enabled)
        return;
      if (self.selectPageCallback)
        self.selectPageCallback(page);
    }
  });
  var item = $("<li/>").append(anchor);
  if (!enabled)
    item.addClass("disabled");
  return item;
};

HatoholPager.prototype.update = function(params) {
  this.applyParams(params);

  var self = this;
  var numPages = this.getTotalNumberOfPages();
  var parent = this.parentElements;
  var i, item, enabled;
  var range = this.getPagesRange();

  parent.empty();
  if (numPages == 0 || numPages == 1)
    return;

  for (i = range.firstPage; i <= range.lastPage; ++i) {
    enabled = true;
    item = this.createItem("" + (i + 1), enabled);
    if (i == this.currentPage)
      item.addClass("active");
    parent.append(item);
  }

  if (numPages > 1 || numPages < 0) {
    enabled = this.currentPage >= self.maxPagesToShow;
    parent.prepend(this.createItem('<i class="glyphicon glyphicon-backward"></i>', enabled, function() {
      return self.currentPage - self.maxPagesToShow;
    }));

    enabled = this.currentPage > 0;
    parent.prepend(this.createItem('<i class="glyphicon glyphicon-step-backward"></i>', enabled, function() {
      return 0;
    }));

    enabled = this.currentPage < (numPages - self.maxPagesToShow);
    parent.append(this.createItem('<i class="glyphicon glyphicon-forward"></i>', enabled, function() {
      return self.currentPage + self.maxPagesToShow;
    }));

    enabled = (this.currentPage != numPages - 1);
    parent.append(this.createItem('<i class="glyphicon glyphicon-step-forward"></i>', enabled, function() {
      return numPages - 1;
    }));
  }

  $(self.numRecordsPerPageEntries).val(self.numRecordsPerPage);
};

var HatoholEventPager = function(params) {
  HatoholPager.apply(this, params);
};

HatoholEventPager.prototype = new HatoholPager;

HatoholEventPager.prototype.update = function(params) {
  this.applyParams(params);

  var self = this;
  var parent = this.parentElements;
  var range = this.getPagesRange();
  var i, item, enabled;
  var numEvents = this.numRecords;

  parent.empty();
  if (numEvents != 0) {
    for (i = range.firstPage; i <= range.lastPage; ++i) {
      enabled = true;
      if (numEvents != this.numRecordsPerPage && i > self.currentPage)
        enabled = false;
      item = this.createItem("" + (i + 1), enabled);
      if (i == this.currentPage)
        item.addClass("active");
      parent.append(item);
    }

    enabled = this.currentPage > 0;
    parent.prepend(this.createItem('<i class="glyphicon glyphicon-chevron-left"></i>', enabled, function() {
      return self.currentPage - 1;
    }));

    self.previousPage = self.currentPage;
    enabled = this.currentPage >= self.maxPagesToShow;
    parent.prepend(this.createItem('<i class="glyphicon glyphicon-backward"></i>', enabled, function() {
      return self.currentPage - self.maxPagesToShow;
    }));

    enabled = (numEvents == this.numRecordsPerPage);
    parent.append(this.createItem('<i class="glyphicon glyphicon-chevron-right"></i>', enabled, function() {
      return self.currentPage + 1;
    }));

    enabled = (numEvents == this.numRecordsPerPage);
    parent.append(this.createItem('<i class="glyphicon glyphicon-forward"></i>', enabled, function() {
      return self.currentPage + self.maxPagesToShow;
    }));

  } else {
    if (self.previousPage == -1) {
        return;
    } else {
      enabled = true;
      parent.append(this.createItem(gettext('Nothing data in this page. Please click here and return.'), enabled, function() {
        return self.previousPage;
      }));
    }
  }

  $(self.numRecordsPerPageEntries).val(self.numRecordsPerPage);
};
