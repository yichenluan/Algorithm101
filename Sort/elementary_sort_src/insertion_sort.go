package main

import (
	"fmt"
	"sort"
)

func InsertionSort(data sort.Interface) {
	n := data.Len()
	for i := 1; i < n; i++ {
		for j := i; j > 0 && data.Less(j, j-1); j-- {
			data.Swap(j, j-1)
		}
	}
}

func main() {
	unorder := sort.IntSlice{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
	fmt.Printf("before order: %v\n", unorder)

	InsertionSort(unorder)

	fmt.Printf("after order: %v\n", unorder)
}
