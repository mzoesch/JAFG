def main(*args, **kwargs) -> None:

    try:
        print("Try")
        return None
    except Exception as e:
        print("Except")
        return None
    finally:
        print("Finally")

    return None

if __name__ == "__main__":
    main("WHY", "FINALLY", "EXECUTED", "?????")
