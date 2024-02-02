from py_nexus import ByteView

def main():
    try:
        print('[TEST] Empty')
        a = ByteView()
        assert a.to_string() == ""
        assert a.to_vector() == []
        assert not bool(a)
        print('[OK]')
    
        print('[TEST] To string')
        a = ByteView("test")
        assert a.to_string() == "test"
        assert a.to_vector() == [ord('t'), ord('e'), ord('s'), ord('t')]
        print('[OK]')

        print('[TEST] To vector')
        a = ByteView([1, 2, 3])
        assert a.to_vector() == [1, 2, 3]
        print('[OK]')

        print('[TEST] Slice')
        a = ByteView("Test 123")
        assert a[-3:].to_string() == "123"
        print('[OK]')

        print('[TEST] Comparison')
        a, b = ByteView("123"), ByteView([ord('1'), ord('2'), ord('3')])
        assert a == b
        print('[OK]')

    except AssertionError as e:
        print(f"AssertionError: {e}")

    except AssertionError as e:
        print(f'--AssertionError: {e}--')
    
    else:
        print("--Assertion success--")


if __name__ == '__main__':
    main()