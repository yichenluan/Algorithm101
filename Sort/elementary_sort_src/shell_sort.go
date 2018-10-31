package main

import (
	"fmt"
	"sort"
)

func ShellSort(data sort.Interface) {
	n := data.Len()
	gap := n / 2
	for gap > 0 {
		for i := gap; i < n; i++ {
			for j := i; j-gap >= 0 && data.Less(j, j-gap); {
				data.Swap(j, j-gap)
				j -= gap
			}
		}
		gap = gap / 2
	}
}

func main() {
	unorder := sort.IntSlice{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
	fmt.Printf("before order: %v\n", unorder)

	ShellSort(unorder)

	fmt.Printf("after order: %v\n", unorder)
}
