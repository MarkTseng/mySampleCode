// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Sorting using a general interface
package sort

// Sorting interface
type Interface interface {
	Len() int
	Less(i, j int) bool
	Swap(i, j int)
}

// General sort function
func Sort(data Interface) {
	for i := 1; i < data.Len(); i++ {
		for j := i; j > 0 && data.Less(j, j-1); j-- {
			data.Swap(j, j-1)
		}
	}
}

// Test if data is sorted
func IsSorted(data Interface) bool {
	n := data.Len()
	for i := n - 1; i > 0; i-- {
		if data.Less(i, i-1) {
			return false
		}
	}
	return true
}

// Convenience types for common cases: IntArray
type IntArray []int

func (p IntArray) Len() int           { return len(p) }
func (p IntArray) Less(i, j int) bool { return p[i] < p[j] }
func (p IntArray) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }

type Float64Array []float64
func (p Float64Array) Len() int           { return len(p) }
func (p Float64Array) Less(i, j int) bool { return p[i] < p[j] }
func (p Float64Array) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }

type StringArray []string
func (p StringArray) Len() int           { return len(p) }
func (p StringArray) Less(i, j int) bool { return p[i] < p[j] }
func (p StringArray) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }

// Convenience wrappers for common cases
func SortInts(a []int)         { Sort(IntArray(a)) }
func SortFloat64s(a []float64) { Sort(Float64Array(a)) }
func SortStrings(a []string)   { Sort(StringArray(a)) }

func IntsAreSorted(a []int) bool         { return IsSorted(IntArray(a)) }
func Float64sAreSorted(a []float64) bool { return IsSorted(Float64Array(a)) }
func StringsAreSorted(a []string) bool   { return IsSorted(StringArray(a)) }